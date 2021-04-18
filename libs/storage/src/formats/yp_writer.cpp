// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/yp_writer.h>

#include <yttrium/base/buffer.h>
#include <yttrium/storage/compressor.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/writer.h>
#include "yp_format.h"

#include <primal/buffer.hpp>

#include <limits>
#include <vector>

namespace Yt
{
	struct YpWriterEntry
	{
		std::string _name;
		explicit YpWriterEntry(std::string_view name)
			: _name{ name } {}
	};

	struct YpWriterData
	{
		Writer _writer;
		std::vector<YpWriterEntry> _entries;
		bool _committed = false;
		bool _finished = false;
		YpCompression _compression = YpCompression::None;
		std::unique_ptr<Compressor> _compressor;
		int _compressionLevel = 0;

		explicit YpWriterData(Writer&& writer) noexcept
			: _writer{ std::move(writer) } {}
	};

	YpWriter::YpWriter(Writer&& writer)
		: _data{ std::make_unique<YpWriterData>(std::move(writer)) }
	{
#if YTTRIUM_COMPRESSION_ZLIB
		_data->_compression = YpCompression::Zlib;
		_data->_compressor = Compressor::zlib();
		_data->_compressionLevel = 9;
#endif
	}

	YpWriter::~YpWriter() noexcept = default;

	bool YpWriter::add(const std::string& path)
	{
		if (_data->_committed || _data->_entries.size() == std::numeric_limits<uint16_t>::max())
			return false;
		_data->_entries.emplace_back(path);
		return true;
	}

	bool YpWriter::commit()
	{
		if (std::exchange(_data->_committed, true))
			return _data->_finished;

		std::vector<YpBlockEntry> indexBlocks;
		indexBlocks.reserve(_data->_entries.size());

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

		for (const auto& entry : _data->_entries)
		{
			if (!writeName(entry._name))
				return false;
			const auto source = Source::from(entry._name);
			if (!source)
				return false;
			auto& block = indexBlocks.emplace_back();
			block._uncompressedSize = static_cast<uint32_t>(source->size());
			primal::Buffer<uint8_t> uncompressedBuffer{ block._uncompressedSize };
			if (!source->read_all_at(0, uncompressedBuffer.data(), block._uncompressedSize))
				return false;
			if (_data->_compressor)
			{
				if (!_data->_compressor->prepare(_data->_compressionLevel))
					return false;
				primal::Buffer<uint8_t> compressedBuffer{ _data->_compressor->maxCompressedSize(block._uncompressedSize) };
				const auto compressedSize = _data->_compressor->compress(compressedBuffer.data(), compressedBuffer.capacity(), uncompressedBuffer.data(), block._uncompressedSize);
				if (compressedSize < block._uncompressedSize)
				{
					block._compressedSize = static_cast<uint32_t>(compressedSize);
					if (!_data->_writer.write_all(compressedBuffer.data(), block._compressedSize))
						return false;
				}
				else
				{
					block._compressedSize = block._uncompressedSize;
					if (!_data->_writer.write_all(uncompressedBuffer.data(), block._uncompressedSize))
						return false;
				}
			}
			else
			{
				block._compressedSize = block._uncompressedSize;
				if (!_data->_writer.write_all(uncompressedBuffer.data(), block._uncompressedSize))
					return false;
			}
		}

		if (!_data->_writer.write_all(indexBlocks.data(), indexBlocks.size() * sizeof(YpBlockEntry))
			|| !_data->_writer.write_all(nameBuffer.data(), nameBuffer.size()))
			return false;

		const auto uncompressedIndexSize = indexBlocks.size() * sizeof(YpBlockEntry) + nameBuffer.size();
		if (uncompressedIndexSize > std::numeric_limits<uint32_t>::max())
			return false;

		header._indexBlock._uncompressedSize = static_cast<uint32_t>(uncompressedIndexSize);
		header._indexBlock._compressedSize = header._indexBlock._uncompressedSize;
		_data->_writer.seek(packageHeaderOffset);
		if (!_data->_writer.write(header))
			return false;

		_data->_finished = true;
		return true;
	}
}
