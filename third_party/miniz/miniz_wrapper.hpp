//===----------------------------------------------------------------------===//
//
//
// miniz_wrapper.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "miniz.hpp"
#include <string>
#include <stdexcept>

namespace bumblebee {

enum class MiniZStreamType {
	MINIZ_TYPE_NONE,
	MINIZ_TYPE_INFLATE,
	MINIZ_TYPE_DEFLATE
};

struct MiniZStream {
	MiniZStream() : type(MiniZStreamType::MINIZ_TYPE_NONE) {
		memset(&stream, 0, sizeof(bumblebee::mz_stream));
	}
	~MiniZStream() {
		switch(type) {
		case MiniZStreamType::MINIZ_TYPE_INFLATE:
			bumblebee::mz_inflateEnd(&stream);
			break;
		case MiniZStreamType::MINIZ_TYPE_DEFLATE:
			bumblebee::mz_deflateEnd(&stream);
			break;
		default:
			break;
		}
	}
	void formatException(std::string error_msg) {
		throw std::runtime_error(error_msg);
	}
	void formatException(const char *error_msg, int mz_ret) {
		auto err = bumblebee::mz_error(mz_ret);
		formatException(error_msg + std::string(": ") + (err ? err : "Unknown error code"));
	}
	void decompress(const char *compressed_data, size_t compressed_size, char *out_data, size_t out_size) {
		auto mz_ret = mz_inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS);
		if (mz_ret != bumblebee::MZ_OK) {
			formatException("Failed to initialize miniz", mz_ret);
		}
		type = MiniZStreamType::MINIZ_TYPE_INFLATE;

		if (compressed_size < GZIP_HEADER_MINSIZE) {
			formatException("Failed to decompress GZIP block: compressed size is less than gzip header size");
		}
		auto gzip_hdr = (const unsigned char *)compressed_data;
		if (gzip_hdr[0] != 0x1F || gzip_hdr[1] != 0x8B || gzip_hdr[2] != GZIP_COMPRESSION_DEFLATE ||
		    gzip_hdr[3] & GZIP_FLAG_UNSUPPORTED) {
			formatException("Input is invalid/unsupported GZIP stream");
		}

		stream.next_in = (const unsigned char *)compressed_data + GZIP_HEADER_MINSIZE;
		stream.avail_in = compressed_size - GZIP_HEADER_MINSIZE;
		stream.next_out = (unsigned char *)out_data;
		stream.avail_out = out_size;

		mz_ret = mz_inflate(&stream, bumblebee::MZ_FINISH);
		if (mz_ret != bumblebee::MZ_OK && mz_ret != bumblebee::MZ_STREAM_END) {
			formatException("Failed to decompress GZIP block", mz_ret);
		}
	}
	size_t MaxCompressedLength(size_t input_size) {
		return bumblebee::mz_compressBound(input_size) + GZIP_HEADER_MINSIZE + GZIP_FOOTER_SIZE;
	}
	void Compress(const char *uncompressed_data, size_t uncompressed_size, char *out_data, size_t *out_size) {
		auto mz_ret = mz_deflateInit2(&stream, bumblebee::MZ_DEFAULT_LEVEL, MZ_DEFLATED, -MZ_DEFAULT_WINDOW_BITS, 1, 0);
		if (mz_ret != bumblebee::MZ_OK) {
			formatException("Failed to initialize miniz", mz_ret);
		}
		type = MiniZStreamType::MINIZ_TYPE_DEFLATE;

		auto gzip_header = (unsigned char*) out_data;
		memset(gzip_header, 0, GZIP_HEADER_MINSIZE);
		gzip_header[0] = 0x1F;
		gzip_header[1] = 0x8B;
		gzip_header[2] = GZIP_COMPRESSION_DEFLATE;
		gzip_header[3] = 0;
		gzip_header[4] = 0;
		gzip_header[5] = 0;
		gzip_header[6] = 0;
		gzip_header[7] = 0;
		gzip_header[8] = 0;
		gzip_header[9] = 0xFF;

		auto gzip_body = gzip_header + GZIP_HEADER_MINSIZE;

		stream.next_in = (const unsigned char*) uncompressed_data;
		stream.avail_in = uncompressed_size;
		stream.next_out = gzip_body;
		stream.avail_out = *out_size - GZIP_HEADER_MINSIZE;

		mz_ret = mz_deflate(&stream, bumblebee::MZ_FINISH);
		if (mz_ret != bumblebee::MZ_OK && mz_ret != bumblebee::MZ_STREAM_END) {
			formatException("Failed to compress GZIP block", mz_ret);
		}
		auto gzip_footer = gzip_body + stream.total_out;
		auto crc = bumblebee::mz_crc32(MZ_CRC32_INIT, (const unsigned char*) uncompressed_data, uncompressed_size);
		gzip_footer[0] = crc & 0xFF;
		gzip_footer[1] = (crc >> 8) & 0xFF;
		gzip_footer[2] = (crc >> 16) & 0xFF;
		gzip_footer[3] = (crc >> 24) & 0xFF;
		gzip_footer[4] = uncompressed_size & 0xFF;
		gzip_footer[5] = (uncompressed_size >> 8) & 0xFF;
		gzip_footer[6] = (uncompressed_size >> 16) & 0xFF;
		gzip_footer[7] = (uncompressed_size >> 24) & 0xFF;

		*out_size = stream.total_out + GZIP_HEADER_MINSIZE + GZIP_FOOTER_SIZE;
	}

private:
	static constexpr uint8_t GZIP_HEADER_MINSIZE = 10;
	static constexpr uint8_t GZIP_FOOTER_SIZE = 8;
	static constexpr uint8_t GZIP_COMPRESSION_DEFLATE = 0x08;
	static constexpr unsigned char GZIP_FLAG_UNSUPPORTED = 0x1 | 0x2 | 0x4 | 0x10 | 0x20;
	bumblebee::mz_stream stream;
	MiniZStreamType type;
};

}
