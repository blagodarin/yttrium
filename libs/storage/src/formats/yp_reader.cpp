// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "yp_reader.h"

#include <yttrium/base/buffer.h>
#include <yttrium/base/numeric.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include "../package.h"
#include "yp_format.h"

#include <cstring>
#include <limits>

#include <fmt/format.h>

namespace Yt
{
	struct YpReader::Entry
	{
		uint64_t _offset = 0;
		uint32_t _size = 0;

		Entry(uint64_t offset, uint32_t size) noexcept
			: _offset{ offset }, _size{ size } {}
	};

	YpReader::YpReader(std::unique_ptr<Source>&& source)
		: _source{ std::move(source) }
	{
		YpFileHeader header;
		if (!_source->read_at(0, header)
			|| header._signature != YpFileHeader::kSignature)
			throw BadPackage{ "Not an Yttrium package" };

		const auto metadataOffset = sizeof header + sizeof(YpFileEntry) * header._entryCount;
		if (header._indexSize < metadataOffset)
			throw BadPackage{ "Bad package header" };

		std::vector<YpFileEntry> entries(size_t{ header._entryCount });
		if (!_source->read_all_at(sizeof header, entries.data(), entries.size() * sizeof(YpFileEntry)))
			throw BadPackage{ "Bad package index" };

		const auto metadataSize = header._indexSize - metadataOffset;
		_metadataBuffer.reserve(metadataSize);
		if (!_source->read_all_at(metadataOffset, _metadataBuffer.data(), metadataSize))
			throw BadPackage{ "Bad package metadata" };

		const auto metadataSource = Source::from(_metadataBuffer.data(), metadataSize);
		Reader metadataReader{ *metadataSource };

		const auto readU8 = [&metadataReader] {
			uint8_t value = 0;
			if (!metadataReader.read(value))
				throw BadPackage{ "Bad package index entry metadata" };
			return value;
		};

		const auto readString = [this, &metadataReader, &readU8] {
			const auto size = readU8();
			if (!metadataReader.skip(size))
				throw BadPackage{ "Bad package index entry metadata" };
			return std::string_view{ _metadataBuffer.data() + metadataReader.offset() - size, size };
		};

		_names.reserve(entries.size());
		_entries.reserve(entries.size());

		const auto sourceSize = _source->size();
		for (const auto& entry : entries)
		{
			const auto index = static_cast<size_t>(&entry - entries.data());
			if (entry._dataOffset > sourceSize || entry._dataSize > sourceSize - entry._dataOffset)
				throw BadPackage{ fmt::format("Bad package index entry #{} data", index) };
			if (entry._metadataOffset < metadataOffset || !metadataReader.seek(entry._metadataOffset - metadataOffset))
				throw BadPackage{ fmt::format("Bad package index entry #{} metadata", index) };
			_names.emplace_back(readString());
			_entries.emplace_back(entry._dataOffset, entry._dataSize);
		}
	}

	YpReader::~YpReader() = default;

	std::unique_ptr<Source> YpReader::open(size_t index) const
	{
		if (index >= _entries.size())
			return {};
		const auto& entry = _entries[index];
		return Source::from(_source, entry._offset, entry._size);
	}
}
