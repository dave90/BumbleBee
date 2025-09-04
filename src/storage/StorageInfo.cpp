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
#include "bumblebee/storage/StorageInfo.h"

#include "bumblebee/common/ErrorHandler.h"
#include "bumblebee/common/serializer/BufferedDeserializer.h"
#include "bumblebee/common/serializer/BufferedSerializer.h"

namespace bumblebee{

const char MainHeader::MAGIC_BYTES[] = "BEEE";

void MainHeader::serialize(Serializer &ser) {
	ser.writeData((data_ptr_t)MAGIC_BYTES, MAGIC_BYTE_SIZE);
	ser.write<uint64_t>(version_number);
	for (idx_t i = 0; i < FLAG_COUNT; i++) {
		ser.write<uint64_t>(flags[i]);
	}
}

void MainHeader::checkMagicBytes(FileHandle &handle) {
	data_t magic_bytes[MAGIC_BYTE_SIZE];
	if (handle.getFileSize() < MainHeader::MAGIC_BYTE_SIZE + MainHeader::MAGIC_BYTE_OFFSET) {
		ErrorHandler::errorGeneric("The file is not a valid database file!");
	}
	handle.read(magic_bytes, MainHeader::MAGIC_BYTE_SIZE, MainHeader::MAGIC_BYTE_OFFSET);
	if (memcmp(magic_bytes, MainHeader::MAGIC_BYTES, MainHeader::MAGIC_BYTE_SIZE) != 0) {
		ErrorHandler::errorGeneric("The file is not a valid database file!");
	}
}

MainHeader MainHeader::deserialize(Deserializer &source) {
	data_t magic_bytes[MAGIC_BYTE_SIZE];
	MainHeader header;
	source.readData(magic_bytes, MainHeader::MAGIC_BYTE_SIZE);
	if (memcmp(magic_bytes, MainHeader::MAGIC_BYTES, MainHeader::MAGIC_BYTE_SIZE) != 0) {
		ErrorHandler::errorGeneric("The file is not a valid database file!");
	}
	header.version_number = source.read<uint64_t>();
	// read the flags
	for (idx_t i = 0; i < FLAG_COUNT; i++) {
		header.flags[i] = source.read<uint64_t>();
	}
	return header;
}

void DatabaseHeader::serialize(Serializer &ser) {
	ser.write<uint64_t>(iteration);
	ser.write<block_id_t>(meta_block);
	ser.write<block_id_t>(free_list);
	ser.write<uint64_t>(block_count);
}

DatabaseHeader DatabaseHeader::deserialize(Deserializer &source) {
	DatabaseHeader header;
	header.iteration = source.read<uint64_t>();
	header.meta_block = source.read<block_id_t>();
	header.free_list = source.read<block_id_t>();
	header.block_count = source.read<uint64_t>();
	return header;
}


}
