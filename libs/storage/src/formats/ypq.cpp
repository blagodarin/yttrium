// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "ypq.h"

#include <yttrium/base/buffer.h>
#include <yttrium/base/numeric.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include "../package.h"

#include <cstring>
#include <limits>

#include <fmt/format.h>

namespace
{
#pragma pack(push, 1)

	struct YpqHeader
	{
		uint32_t _signature = 0;
		uint32_t _entryCount = 0;
		uint32_t _indexSize = 0;
		uint32_t _reserved = 0;
	};

	struct YpqEntry
	{
		uint64_t _dataOffset = 0;
		uint32_t _dataSize = 0;
		uint32_t _metadataOffset = 0;
	};

#pragma pack(pop)
}

namespace Yt
{
	struct YpqReader::Entry
	{
		uint64_t _offset = 0;
		uint32_t _size = 0;

		Entry(uint64_t offset, uint32_t size) noexcept
			: _offset{ offset }, _size{ size } {}
	};

	YpqReader::YpqReader(std::unique_ptr<Source>&& source)
		: _source{ std::move(source) }
	{
		YpqHeader header;
		if (!_source->read_at(0, header)
			|| header._signature != kYpqSignature)
			throw BadPackage{ "Not an YPQ package" };

		const auto metadataOffset = sizeof header + sizeof(YpqEntry) * header._entryCount;
		if (header._indexSize < metadataOffset)
			throw BadPackage{ "Bad package header" };

		std::vector<YpqEntry> entries(size_t{ header._entryCount });
		if (!_source->read_all_at(sizeof header, entries.data(), entries.size() * sizeof(YpqEntry)))
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

	YpqReader::~YpqReader() = default;

	std::unique_ptr<Source> YpqReader::open(size_t index) const
	{
		if (index >= _entries.size())
			return {};
		const auto& entry = _entries[index];
		return Source::from(_source, entry._offset, entry._size);
	}

	struct YpqWriter::Entry
	{
		std::string _name;

		explicit Entry(std::string_view name)
			: _name{ name } {}
	};

	YpqWriter::YpqWriter(Writer&& writer)
		: _writer{ std::move(writer) } {}

	YpqWriter::~YpqWriter() = default;

	bool YpqWriter::add(const std::string& path)
	{
		if (_committed)
			return false;
		_entries.emplace_back(path);
		return true;
	}

	bool YpqWriter::commit()
	{
		if (std::exchange(_committed, true))
			return false;

		std::vector<YpqEntry> entries;

		const auto metadataOffset = sizeof(YpqHeader) + sizeof(YpqEntry) * _entries.size();

		Buffer metadataBuffer;
		{
			Writer writer{ metadataBuffer };

			const auto write_string = [&writer](const std::string& value) {
				const auto size = static_cast<uint8_t>(value.size()); //-V1029
				return size == value.size() && writer.write(size) && writer.write(value.data(), size);
			};

			for (const auto& entry : _entries)
			{
				entries.emplace_back()._metadataOffset = static_cast<uint32_t>(metadataOffset + writer.offset());
				if (!write_string(entry._name))
					return false;
			}
		}

		const auto dataOffset = metadataOffset + metadataBuffer.size();

		for (auto& entry : entries)
			entry._dataOffset = dataOffset;

		YpqHeader header;
		header._signature = kYpqSignature;
		header._entryCount = static_cast<uint32_t>(_entries.size());
		header._indexSize = static_cast<uint32_t>(dataOffset);
		if (!_writer.write(header))
			return false;

		if (_writer.write(entries.data(), entries.size() * sizeof(YpqEntry)) != entries.size() * sizeof(YpqEntry))
			return false;

		if (_writer.write(metadataBuffer.data(), metadataBuffer.size()) != metadataBuffer.size())
			return false;

		for (const auto& entry : _entries)
		{
			const auto source = Source::from(entry._name);
			if (!source)
				return false;
			const auto i = static_cast<size_t>(&entry - _entries.data());
			entries[i]._dataOffset = _writer.offset();
			entries[i]._dataSize = static_cast<uint32_t>(source->size());
			if (entries[i]._dataSize != source->size())
				return false;
			if (!_writer.write_all(*source))
				return false;
		}

		_writer.seek(sizeof(YpqHeader));
		if (_writer.write(entries.data(), entries.size() * sizeof(YpqEntry)) != entries.size() * sizeof(YpqEntry))
			return false;

		_finished = true;
		return true;
	}
}
