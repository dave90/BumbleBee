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
#include "ResizeableBuffer.hpp"
#include "bumblebee/common/Helper.hpp"

namespace bumblebee{


class RleBpDecoder {
public:
	RleBpDecoder(const uint8_t *buffer, uint32_t buffer_len, uint32_t bit_width)
	    : buffer_((char *)buffer, buffer_len), bitWidth_(bit_width), currentValue_(0), repeatCount_(0),
	      literalCount_(0) {
		if (bit_width >= 64) {
			ErrorHandler::errorGeneric("Decode bit width too large");
		}
		byteEncodedLen_ = ((bitWidth_ + 7) / 8);
		maxVal_ = (1 << bitWidth_) - 1;
	}

	template <typename T>
	void getBatch(char *values_target_ptr, uint32_t batch_size) {
		auto values = (T *)values_target_ptr;
		uint32_t values_read = 0;

		while (values_read < batch_size) {
			if (repeatCount_ > 0) {
				int repeat_batch = minValue(batch_size - values_read, static_cast<uint32_t>(repeatCount_));
				std::fill(values + values_read, values + values_read + repeat_batch, static_cast<T>(currentValue_));
				repeatCount_ -= repeat_batch;
				values_read += repeat_batch;
			} else if (literalCount_ > 0) {
				uint32_t literal_batch = minValue(batch_size - values_read, static_cast<uint32_t>(literalCount_));
				uint32_t actual_read = bitUnpack<T>(values + values_read, literal_batch);
				if (literal_batch != actual_read) {
					ErrorHandler::errorGeneric("Did not find enough values");
				}
				literalCount_ -= literal_batch;
				values_read += literal_batch;
			} else {
				if (!nextCounts<T>()) {
					if (values_read != batch_size) {
						ErrorHandler::errorGeneric("RLE decode did not find enough values");
					}
					return;
				}
			}
		}
		if (values_read != batch_size) {
			ErrorHandler::errorGeneric("RLE decode did not find enough values");
		}
	}

private:
	ByteBuffer buffer_;

	int bitWidth_;
	uint64_t currentValue_;
	uint32_t repeatCount_;
	uint32_t literalCount_;
	uint8_t byteEncodedLen_;
	uint32_t maxVal_;

	int8_t bitpackPos_ {0};

	// this is slow but whatever, calls are rare
	uint32_t varintDecode() {
		uint32_t result = 0;
		uint8_t shift = 0;
		uint8_t len = 0;
		while (true) {
			auto byte = buffer_.read<uint8_t>();
			len++;
			result |= (byte & 127) << shift;
			if ((byte & 128) == 0)
				break;
			shift += 7;
			if (shift > 32) {
				ErrorHandler::errorGeneric("Varint-decoding found too large number");
			}
		}
		return result;
	}

	template <typename T>
	bool nextCounts() {
		// Read the next run's indicator int, it could be a literal or repeated run.
		// The int is encoded as a vlq-encoded value.
		if (bitpackPos_ != 0) {
			buffer_.inc(1);
			bitpackPos_ = 0;
		}
		auto indicator_value = varintDecode();

		// lsb indicates if it is a literal run or repeated run
		bool is_literal = indicator_value & 1;
		if (is_literal) {
			literalCount_ = (indicator_value >> 1) * 8;
		} else {
			repeatCount_ = indicator_value >> 1;
			// (ARROW-4018) this is not big-endian compatible, lol
			currentValue_ = 0;
			for (auto i = 0; i < byteEncodedLen_; i++) {
				currentValue_ |= (buffer_.read<uint8_t>() << (i * 8));
			}
			// sanity check
			if (repeatCount_ > 0 && currentValue_ > maxVal_) {
				ErrorHandler::errorGeneric("Payload value bigger than allowed. Corrupted file?");
			}
		}
		// TODO complain if we run out of buffer
		return true;
	}

	// somewhat optimized implementation that avoids non-alignment

	static const uint32_t BITPACK_MASKS[];
	static const uint8_t BITPACK_DLEN;

	template <typename T>
	uint32_t bitUnpack(T *dest, uint32_t count) {
		auto mask = BITPACK_MASKS[bitWidth_];

		for (uint32_t i = 0; i < count; i++) {
			T val = (buffer_.get<uint8_t>() >> bitpackPos_) & mask;
			bitpackPos_ += bitWidth_;
			while (bitpackPos_ > BITPACK_DLEN) {
				buffer_.inc(1);
				val |= (buffer_.get<uint8_t>() << (BITPACK_DLEN - (bitpackPos_ - bitWidth_))) & mask;
				bitpackPos_ -= BITPACK_DLEN;
			}
			dest[i] = val;
		}
		return count;
	}
};


}
