// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "yp_reader.h"

#include <yttrium/base/buffer.h>
#include <yttrium/base/numeric.h>
#include <yttrium/storage/source.h>
#include "../package.h"
#include "yp_format.h"

#include <cstring>
#include <limits>

namespace Yt
{
	struct YpReader::Entry
	{
		uint64_t _offset = 0;
		uint32_t _compressedSize = 0;
		uint32_t _uncompressedSize = 0;

		constexpr Entry(uint64_t offset, uint32_t compressedSize, uint32_t uncompressedSize) noexcept
			: _offset{ offset }, _compressedSize{ compressedSize }, _uncompressedSize{ uncompressedSize } {}
	};

	YpReader::YpReader(std::unique_ptr<Source>&& source)
		: _source{ std::move(source) }
	{
		YpPackageHeader header;
		if (!_source->read_at(0, header)
			|| header._signature != YpPackageHeader::kSignature
			|| header._compression != YpCompression::None
			|| header._reserved != 0
			|| header._indexBlock._compressedSize > _source->size() - sizeof header
			|| header._fileCount > header._indexBlock._uncompressedSize / sizeof(YpBlockEntry))
			throw BadPackage{ "Invalid Yttrium package" };
		const auto indexOffset = _source->size() - header._indexBlock._compressedSize;
		_indexBuffer.reserve(header._indexBlock._uncompressedSize);
		if (header._indexBlock._compressedSize == header._indexBlock._uncompressedSize)
		{
			if (!_source->read_at(indexOffset, _indexBuffer.data(), header._indexBlock._uncompressedSize))
				throw BadPackage{ "Invalid Yttrium package" };
		}
		else
		{
			if (header._indexBlock._compressedSize == 0 || header._indexBlock._uncompressedSize == 0)
				throw BadPackage{ "Invalid Yttrium package" };
			primal::Buffer<uint8_t> compressedIndexBuffer{ header._indexBlock._compressedSize };
			if (!_source->read_at(indexOffset, compressedIndexBuffer.data(), header._indexBlock._compressedSize))
				throw BadPackage{ "Invalid Yttrium package" };
			// TODO: Decompress.
		}
		std::span<const YpBlockEntry> entries{ reinterpret_cast<YpBlockEntry*>(_indexBuffer.data()), header._fileCount };
		std::span<const uint8_t> metadata{ _indexBuffer.data() + header._fileCount * sizeof(YpBlockEntry), header._indexBlock._uncompressedSize - header._fileCount * sizeof(YpBlockEntry) };
		_entries.reserve(entries.size());
		_names.reserve(entries.size());
		uint64_t offset = sizeof header;
		for (const auto& entry : entries)
		{
			if (entry._compressedSize != entry._uncompressedSize
				|| entry._compressedSize > indexOffset - offset)
				throw BadPackage{ "Invalid Yttrium package" };
			if (metadata.size() < 1 || metadata.size() < 1 + metadata[0])
				throw BadPackage{ "Invalid Yttrium package" };
			_entries.emplace_back(offset, entry._compressedSize, entry._uncompressedSize);
			_names.emplace_back(reinterpret_cast<const char*>(metadata.data() + 1), metadata[0]);
			offset += entry._compressedSize;
			metadata = metadata.subspan(1 + metadata[0]);
		}
	}

	YpReader::~YpReader() = default;

	std::unique_ptr<Source> YpReader::open(size_t index) const
	{
		if (index >= _entries.size())
			return {};
		const auto& entry = _entries[index];
		if (entry._compressedSize == entry._uncompressedSize)
			return Source::from(_source, entry._offset, entry._uncompressedSize);
		return {}; // TODO: Decompress.
	}
}
