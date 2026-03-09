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
#pragma once
#include <memory>

#include "bumblebee/common/FileSystem.hpp"
#include "bumblebee/common/serializer/BufferedFileWriter.hpp"
#include "bumblebee/common/types/ChunkCollection.hpp"
#include "parquet/parquet_types.h"


namespace bumblebee {

class ParquetWriter {
public:
    ParquetWriter(FileSystem &fs, string file_name, vector<LogicalType> types,
                  vector<string> names, format::CompressionCodec::type codec);

public:
    void flush(ChunkCollection &buffer);
    void finalize();

private:
    string fileName_;
    vector<LogicalType> sqlTypes_;
    vector<string> columnNames_;
    format::CompressionCodec::type codec_;

    std::unique_ptr<BufferedFileWriter> writer_;
    std::shared_ptr<thrift::protocol::TProtocol> protocol_;
    format::FileMetaData fileMetaData_;
    std::mutex lock_;
    Vector allOne_;
};


using parquet_writer_ptr_t = std::unique_ptr<ParquetWriter>;

}
