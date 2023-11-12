// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/yp_writer.h>

#include <yttrium/base/buffer.h>
#include <yttrium/storage/compressor.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>
#include "yp_format.h"

#include <seir_base/buffer.hpp>

#include <cstring>
#include <limits>
#include <vector>

namespace Yt
{
	struct YpWriterEntry
	{
		std::string _name;
		int _compressionLevel = 0;
		explicit YpWriterEntry(std::string_view name, int compressionLevel)
			: _name{ name }, _compressionLevel{ compressionLevel } {}
	};

	struct YpWriterData
	{
		Writer _writer;
		std::vector<YpWriterEntry> _entries;
		bool _committed = false;
		bool _finished = false;
		YpCompression _compression = YpCompression::None;
		std::unique_ptr<Compressor> _compressor;
		int _minCompressionLevel = 0;
		int _maxCompressionLevel = 0;

		explicit YpWriterData(Writer&& writer) noexcept
			: _writer{ std::move(writer) } {}
	};

	YpWriter::YpWriter(Writer&& writer, Compression compression)
		: _data{ std::make_unique<YpWriterData>(std::move(writer)) }
	{
		switch (compression)
		{
		case Compression::None:
			break;
		case Compression::Zlib:
#if YTTRIUM_COMPRESSION_ZLIB
			_data->_compression = YpCompression::Zlib;
			_data->_compressor = Compressor::zlib();
			_data->_minCompressionLevel = 0;
			_data->_maxCompressionLevel = 9;
			// TODO: Get compression level bounds from the compressor.
#endif
			// TODO: Fail if the requested compressor in unavaliable.
			break;
		}
	}

	YpWriter::~YpWriter() noexcept = default;

	bool YpWriter::add(const std::string& path, int compressionLevel)
	{
		if (_data->_committed
			|| _data->_entries.size() == std::numeric_limits<uint16_t>::max()
			|| compressionLevel < _data->_minCompressionLevel
			|| compressionLevel > _data->_maxCompressionLevel)
			return false;
		_data->_entries.emplace_back(path, compressionLevel);
		return true;
	}

	bool YpWriter::commit()
	{
		if (std::exchange(_data->_committed, true))
			return _data->_finished;

		const auto strippedIndexSize = _data->_entries.size() * sizeof(YpBlockEntry);
		seir::Buffer indexBuffer{ strippedIndexSize };
		auto indexEntry = reinterpret_cast<YpBlockEntry*>(indexBuffer.data());

		Buffer nameBuffer;
		Writer nameWriter{ nameBuffer };
		const auto writeName = [&nameWriter](const std::string& value) {
			const auto size = static_cast<uint8_t>(value.size());
			return size == value.size() && nameWriter.write(size) && nameWriter.write(value.data(), size) == size;
		};

		const auto packageHeaderOffset = _data->_writer.offset();

		YpPackageHeader header;
		header._fileCount = static_cast<uint16_t>(_data->_entries.size());
		header._compression = _data->_compression;
		if (!_data->_writer.write(header))
			return false;

		seir::Buffer compressedBuffer;
		const auto writeBlock = [this, &compressedBuffer](YpBlockEntry& block, const void* data, size_t size, int compressionLevel) {
			if (size > std::numeric_limits<uint32_t>::max())
				return false;
			if (compressionLevel != 0 && _data->_compressor)
			{
				if (!_data->_compressor->prepare(compressionLevel))
					return false;
				compressedBuffer.reserve(_data->_compressor->maxCompressedSize(size), 0);
				const auto compressedSize = _data->_compressor->compress(compressedBuffer.data(), compressedBuffer.capacity(), data, size);
				if (compressedSize < size)
				{
					block._compressedSize = static_cast<uint32_t>(compressedSize);
					block._uncompressedSize = static_cast<uint32_t>(size);
					return _data->_writer.write_all(compressedBuffer.data(), compressedSize);
				}
			}
			block._compressedSize = static_cast<uint32_t>(size);
			block._uncompressedSize = block._compressedSize;
			return _data->_writer.write_all(data, block._uncompressedSize);
		};

		seir::Buffer uncompressedBuffer;
		for (const auto& entry : _data->_entries)
		{
			if (!writeName(entry._name))
				return false;
			const auto source = Source::from(entry._name);
			if (!source)
				return false;
			const auto uncompressedSize = static_cast<size_t>(source->size());
			uncompressedBuffer.reserve(uncompressedSize, false);
			if (!source->read_all_at(0, uncompressedBuffer.data(), uncompressedSize) || !writeBlock(*indexEntry, uncompressedBuffer.data(), uncompressedSize, entry._compressionLevel))
				return false;
			++indexEntry;
		}

		const auto totalIndexSize = strippedIndexSize + nameBuffer.size();
		indexBuffer.reserve(totalIndexSize, 0);
		std::memcpy(indexBuffer.data() + strippedIndexSize, nameBuffer.data(), nameBuffer.size());
		if (!writeBlock(header._indexBlock, indexBuffer.data(), totalIndexSize, _data->_maxCompressionLevel))
			return false;

		_data->_writer.seek(packageHeaderOffset);
		if (!_data->_writer.write(header))
			return false;

		_data->_finished = true;
		return true;
	}
}
