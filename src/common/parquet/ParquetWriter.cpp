/*
 * Copyright (C) 2025 Davide Fuscà
 *
 * This file is part of BumbleBee.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "bumblebee/common/parquet/ParquetWriter.hpp"

#include "bumblebee/common/Limits.hpp"
#include "bumblebee/common/parquet/ColumnReader.hpp"
#include "bumblebee/common/parquet/ParquetTimestamp.hpp"
#include "bumblebee/common/serializer/BufferedSerializer.hpp"
#include "bumblebee/common/types/Timestamp.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"
#include "miniz/miniz_wrapper.hpp"
#include "snappy/snappy.h"
#include "thrift/protocol/TCompactProtocol.h"
#include "zstd/include/zstd.h"

namespace bumblebee{

using ParquetRowGroup = format::RowGroup;


class MyTransport : public thrift::transport::TTransport {
public:
    explicit MyTransport(Serializer &serializer) : serializer(serializer) {
    }

    bool isOpen() const override {
        return true;
    }

    void open() override {
    }

    void close() override {
    }

    void write_virt(const uint8_t *buf, uint32_t len) override {
        serializer.writeData((const_data_ptr_t)buf, len);
    }

private:
    Serializer &serializer;
};

static Type::type bumblebeeTypeToParquetType(const LogicalType &type) {
    switch (type.type()) {
        case LogicalTypeId::BOOLEAN:
            return Type::BOOLEAN;
        case LogicalTypeId::TINYINT:
        case LogicalTypeId::SMALLINT:
        case LogicalTypeId::INTEGER:
            return Type::INT32;
        case LogicalTypeId::BIGINT:
            return Type::INT64;
        case LogicalTypeId::FLOAT:
            return Type::FLOAT;
        case LogicalTypeId::DECIMAL:
        case LogicalTypeId::DOUBLE:
            return Type::DOUBLE;
        case LogicalTypeId::STRING:
            return Type::BYTE_ARRAY;
        case LogicalTypeId::DATE:
        case LogicalTypeId::TIMESTAMP:
            return Type::INT96;
        default:
            ErrorHandler::errorNotImplemented("Parquet write type not supported: "+type.toString());
    }
	return Type::INT96;
}



static bool bumblebeeTypeToConvertedType(const LogicalType &type, format::ConvertedType::type &result) {
    switch (type.type()) {
        case LogicalTypeId::STRING:
            result = format::ConvertedType::UTF8;
            return true;
        default:
            return false;
    }
}

static void varintEncode(uint32_t val, Serializer &ser) {
    do {
        uint8_t byte = val & 127;
        val >>= 7;
        if (val != 0) {
            byte |= 128;
        }
        ser.write<uint8_t>(byte);
    } while (val != 0);
}

static uint8_t getVarintSize(uint32_t val) {
    uint8_t res = 0;
    do {
        uint8_t byte = val & 127;
        val >>= 7;
        if (val != 0) {
            byte |= 128;
        }
        res++;
    } while (val != 0);
    return res;
}

template <class SRC, class TGT>
static void templatedWritePlain(Vector &col, idx_t length, Serializer &ser) {
    auto *ptr = FlatVector::getData<SRC>(col);
    for (idx_t r = 0; r < length; r++) {
        ser.write<TGT>((TGT)ptr[r]);
    }
}

ParquetWriter::ParquetWriter(FileSystem &fs, string file_name, vector<LogicalType> types,
    vector<string> names, format::CompressionCodec::type codec)
    :fileName_(std::move(file_name)), sqlTypes_(std::move(types)), columnNames_(std::move(names)), codec_(codec), allOne_(LogicalTypeId::UBIGINT, STANDARD_VECTOR_SIZE) {
    // initialize the file writer
    writer_ = std::make_unique<BufferedFileWriter>(fs, fileName_.c_str(), FileFlags::FILE_FLAGS_WRITE | FileFlags::FILE_FLAGS_FILE_CREATE_NEW);
    // parquet files start with the string "PAR1"
    writer_->writeData((const_data_ptr_t) "PAR1", 4);
    thrift::protocol::TCompactProtocolFactoryT<MyTransport> tproto_factory;
    protocol_ = tproto_factory.getProtocol(std::make_shared<MyTransport>(*writer_));

    fileMetaData_.num_rows = 0;
    fileMetaData_.version = 1;

    fileMetaData_.__isset.created_by = true;
    fileMetaData_.created_by = "BumbleBee";

    fileMetaData_.schema.resize(sqlTypes_.size() + 1);

    // populate root schema object
    fileMetaData_.schema[0].name = "bumblebee_schema";
    fileMetaData_.schema[0].num_children = sqlTypes_.size();
    fileMetaData_.schema[0].__isset.num_children = true;

    for (idx_t i = 0; i < sqlTypes_.size(); i++) {
        auto &schema_element = fileMetaData_.schema[i + 1];

        schema_element.type = bumblebeeTypeToParquetType(sqlTypes_[i]);
        schema_element.repetition_type = FieldRepetitionType::OPTIONAL;
        schema_element.num_children = 0;
        schema_element.__isset.num_children = true;
        schema_element.__isset.type = true;
        schema_element.__isset.repetition_type = true;
        schema_element.name = columnNames_[i];
        schema_element.__isset.converted_type = bumblebeeTypeToConvertedType(sqlTypes_[i], schema_element.converted_type);
    }

	// init the all one vector
	auto data = FlatVector::getData(allOne_);
	for (idx_t i = 0; i< STANDARD_VECTOR_SIZE;++i)
		data[i] = NumericLimits<uint64_t>::maximum();
}

void bumblebee::ParquetWriter::flush(ChunkCollection &buffer) {
    if (buffer.getCount() == 0) {
		return;
	}
	std::lock_guard glock(lock_);

	// set up a new row group for this chunk collection
	ParquetRowGroup row_group;
	row_group.num_rows = 0;
	row_group.file_offset = writer_->getTotalWritten();
	row_group.__isset.file_offset = true;
	row_group.columns.resize(buffer.columnCount());

	Vector allOne(LogicalTypeId::UBIGINT, STANDARD_VECTOR_SIZE);


	// iterate over each of the columns of the chunk collection and write them
	for (idx_t i = 0; i < buffer.columnCount(); i++) {
		// we start off by writing everything into a temporary buffer
		// this is necessary to (1) know the total written size, and (2) to compress it afterwards
		BufferedSerializer temp_writer;

		// set up some metadata
		PageHeader hdr;
		hdr.compressed_page_size = 0;
		hdr.uncompressed_page_size = 0;
		hdr.type = format::PageType::DATA_PAGE;
		hdr.__isset.data_page_header = true;

		hdr.data_page_header.num_values = buffer.getCount();
		hdr.data_page_header.encoding = format::Encoding::PLAIN;
		hdr.data_page_header.definition_level_encoding = format::Encoding::RLE;
		hdr.data_page_header.repetition_level_encoding = format::Encoding::BIT_PACKED;

		// record the current offset of the writer into the file
		// this is the starting position of the current page
		auto start_offset = writer_->getTotalWritten();

		// write the definition levels
		// we always bit pack everything

		// first figure out how many bytes we need (1 byte per 8 rows, rounded up)
		auto define_byte_count = (buffer.getCount() + 7) / 8;
		// we need to set up the count as a varint, plus an added marker for the RLE scheme
		// for this marker we shift the count left 1 and set low bit to 1 to indicate bit packed literals
		uint32_t define_header = (define_byte_count << 1) | 1;
		uint32_t define_size = getVarintSize(define_header) + define_byte_count;

		// we write the actual definitions into the temp_writer for now
		temp_writer.write<uint32_t>(define_size);
		varintEncode(define_header, temp_writer);


		for (auto &chunk : buffer.chunks()) {
			// TODO write the null checking the limits
			// write that all the rows are valid
			BB_ASSERT(chunk->getSize() <= STANDARD_VECTOR_SIZE);
			auto data = FlatVector::getData(allOne_);
			auto chunk_define_byte_count = (chunk->getSize() + 7) / 8;
			temp_writer.writeData((const_data_ptr_t)data, chunk_define_byte_count);
		}

		// now write the actual payload: we write this as PLAIN values
		for (auto &chunk : buffer.chunks()) {
			auto &input = *chunk;
			auto &input_column = input.data_[i];

			// write actual payload data
			switch (sqlTypes_[i].type()) {
			case LogicalTypeId::BOOLEAN: {
				auto *ptr = FlatVector::getData<uint8_t>(input_column);
				uint8_t byte = 0;
				uint8_t byte_pos = 0;
				for (idx_t r = 0; r < input.getSize(); r++) {
					byte |= (ptr[r] & 1) << byte_pos;
					byte_pos++;

					if (byte_pos == 8) {
						temp_writer.write<uint8_t>(byte);
						byte = 0;
						byte_pos = 0;
					}
				}
				// flush last byte if req
				if (byte_pos > 0) {
					temp_writer.write<uint8_t>(byte);
				}
				break;
			}
			case LogicalTypeId::TINYINT:
				templatedWritePlain<int8_t, int32_t>(input_column, input.getSize(), temp_writer);
				break;
			case LogicalTypeId::SMALLINT:
				templatedWritePlain<int16_t, int32_t>(input_column, input.getSize(), temp_writer);
				break;
			case LogicalTypeId::INTEGER:
				templatedWritePlain<int32_t, int32_t>(input_column, input.getSize(), temp_writer);
				break;
			case LogicalTypeId::BIGINT:
				templatedWritePlain<int64_t, int64_t>(input_column, input.getSize(), temp_writer);
				break;
			case LogicalTypeId::FLOAT:
				templatedWritePlain<float, float>(input_column, input.getSize(), temp_writer);
				break;
			case LogicalTypeId::DECIMAL: {
				// FIXME: fixed length byte array...
				Vector double_vec(LogicalTypeId::DOUBLE);
				VectorOperations::cast(input_column, double_vec, input.getSize());
				templatedWritePlain<double, double>(double_vec, input.getSize(), temp_writer);
				break;
			}
			case LogicalTypeId::DOUBLE:
				templatedWritePlain<double, double>(input_column, input.getSize(), temp_writer);
				break;
			case LogicalTypeId::DATE: {
				auto *ptr = FlatVector::getData<date_t>(input_column);
				for (idx_t r = 0; r < input.getSize(); r++) {
					auto ts = Timestamp::fromDatetime(ptr[r]);
					temp_writer.write<Int96>(timestampToImpalaTimestamp(ts));
				}
				break;
			}
			case LogicalTypeId::TIMESTAMP: {
				auto *ptr = FlatVector::getData<timestamp_t>(input_column);
				for (idx_t r = 0; r < input.getSize(); r++) {
					temp_writer.write<Int96>(timestampToImpalaTimestamp(ptr[r]));
				}
				break;
			}
			case LogicalTypeId::STRING: {
				auto *ptr = FlatVector::getData<string_t>(input_column);
				for (idx_t r = 0; r < input.getSize(); r++) {
					temp_writer.write<uint32_t>(ptr[r].size());
					temp_writer.writeData((const_data_ptr_t)ptr[r].getDataUnsafe(), ptr[r].size());
				}
				break;
			}
				default:
				ErrorHandler::errorNotImplemented("Parquet write type not supported:" + (sqlTypes_[i].toString()));
			}
		}

		// now that we have finished writing the data we know the uncompressed size
		hdr.uncompressed_page_size = temp_writer.blob_.size_;

		// compress the data based
		size_t compressed_size;
		data_ptr_t compressed_data;
		std::unique_ptr<data_t[]> compressed_buf;
		switch (codec_) {
		case format::CompressionCodec::UNCOMPRESSED:
			compressed_size = temp_writer.blob_.size_;
			compressed_data = temp_writer.blob_.data_.get();
			break;
		case format::CompressionCodec::SNAPPY: {
			compressed_size = snappy::MaxCompressedLength(temp_writer.blob_.size_);
			compressed_buf = std::unique_ptr<data_t[]>(new data_t[compressed_size]);
			snappy::RawCompress((const char *)temp_writer.blob_.data_.get(), temp_writer.blob_.size_,
			                    (char *)compressed_buf.get(), &compressed_size);
			compressed_data = compressed_buf.get();
			break;
		}
		case format::CompressionCodec::GZIP: {
			MiniZStream s;
			compressed_size = s.MaxCompressedLength(temp_writer.blob_.size_);
			compressed_buf = std::unique_ptr<data_t[]>(new data_t[compressed_size]);
			s.Compress((const char *)temp_writer.blob_.data_.get(), temp_writer.blob_.size_, (char *)compressed_buf.get(),
			           &compressed_size);
			compressed_data = compressed_buf.get();
			break;
		}
		case format::CompressionCodec::ZSTD: {
			compressed_size = ZSTD_compressBound(temp_writer.blob_.size_);
			compressed_buf = std::unique_ptr<data_t[]>(new data_t[compressed_size]);
			compressed_size = ZSTD_compress((void *)compressed_buf.get(), compressed_size,
			                                             (const void *)temp_writer.blob_.data_.get(),
			                                             temp_writer.blob_.size_, ZSTD_CLEVEL_DEFAULT);
			compressed_data = compressed_buf.get();
			break;
		}
			default:
			ErrorHandler::errorNotImplemented("Unsupported codec for Parquet Writer");
		}

		hdr.compressed_page_size = compressed_size;
		// now finally write the data to the actual file
		hdr.write(protocol_.get());
		writer_->writeData(compressed_data, compressed_size);

		auto &column_chunk = row_group.columns[i];
		column_chunk.__isset.meta_data = true;
		column_chunk.meta_data.data_page_offset = start_offset;
		column_chunk.meta_data.total_compressed_size = writer_->getTotalWritten() - start_offset;
		column_chunk.meta_data.codec = codec_;
		column_chunk.meta_data.path_in_schema.push_back(fileMetaData_.schema[i + 1].name);
		column_chunk.meta_data.num_values = buffer.getCount();
		column_chunk.meta_data.type = fileMetaData_.schema[i + 1].type;
	}
	row_group.num_rows += buffer.getCount();

	// append the row group to the file meta data
	fileMetaData_.row_groups.push_back(row_group);
	fileMetaData_.num_rows += buffer.getCount();
}

void ParquetWriter::finalize() {
	auto start_offset = writer_->getTotalWritten();
	fileMetaData_.write(protocol_.get());

	writer_->write<uint32_t>(writer_->getTotalWritten() - start_offset);

	// parquet files also end with the string "PAR1"
	writer_->writeData((const_data_ptr_t) "PAR1", 4);

	// flush to disk
	writer_->sync();
	writer_.reset();
}


}
