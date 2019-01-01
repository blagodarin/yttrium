//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "ypq.h"

#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include "../../utils/fourcc.h"
#include "../package.h"

#include <cstring>
#include <limits>

namespace
{
	using namespace Yttrium::Literals;

#pragma pack(push, 1)

	struct YpqHeader
	{
		std::uint32_t signature = 0;
		std::uint32_t entry_count = 0;
		std::uint32_t index_size = 0;
		std::uint32_t reserved = 0;

		static constexpr auto Signature = "\xDFYPQ"_fourcc;
	};

	struct YpqEntry
	{
		std::uint64_t data_offset = 0;
		std::uint32_t data_size = 0;
		std::uint32_t metadata_offset = 0;
	};

#pragma pack(pop)
}

namespace Yttrium
{
	struct YpqReader::Entry
	{
		std::uint64_t _offset = 0;
		std::uint32_t _size = 0;

		Entry(std::uint64_t offset, std::uint32_t size) noexcept
			: _offset{ offset }, _size{ size } {}
	};

	YpqReader::YpqReader(std::unique_ptr<Source>&& source)
		: _source{ std::move(source) }
	{
		YpqHeader header;
		if (!_source->read_at(0, header)
			|| header.signature != YpqHeader::Signature)
			throw BadPackage{ "Not an YPQ package" };

		const auto metadata_offset = sizeof header + sizeof(YpqEntry) * header.entry_count;
		if (header.index_size < metadata_offset)
			throw BadPackage{ "Bad package header" };

		std::vector<YpqEntry> entries(size_t{ header.entry_count });
		if (!_source->read_all_at(sizeof header, entries.data(), entries.size() * sizeof(YpqEntry)))
			throw BadPackage{ "Bad package index" };

		_metadata_buffer.reset(size_t{ header.index_size - metadata_offset });
		if (!_source->read_all_at(metadata_offset, _metadata_buffer.data(), _metadata_buffer.size()))
			throw BadPackage{ "Bad package metadata" };

		const auto metadata_source = Source::from(_metadata_buffer.data(), _metadata_buffer.size());
		Reader metadata_reader{ *metadata_source };

		const auto read_uint8 = [&metadata_reader] {
			uint8_t value = 0;
			if (!metadata_reader.read(value))
				throw BadPackage{ "Bad package index entry metadata" };
			return value;
		};

		const auto read_string = [this, &metadata_reader, &read_uint8] {
			const auto size = read_uint8();
			if (!metadata_reader.skip(size))
				throw BadPackage{ "Bad package index entry metadata" };
			return std::string_view{ static_cast<const char*>(_metadata_buffer.data()) + metadata_reader.offset() - size, size };
		};

		_names.reserve(entries.size());
		_entries.reserve(entries.size());

		const auto source_size = _source->size();
		for (const auto& entry : entries)
		{
			const auto index = static_cast<std::size_t>(&entry - entries.data());
			if (entry.data_offset > source_size || entry.data_offset + entry.data_size > source_size)
				throw BadPackage{ "Bad package index entry #", index, " data" };
			if (entry.metadata_offset < metadata_offset || !metadata_reader.seek(entry.metadata_offset - metadata_offset))
				throw BadPackage{ "Bad package index entry #", index, " metadata" };
			_names.emplace_back(read_string());
			_entries.emplace_back(entry.data_offset, entry.data_size);
		}
	}

	YpqReader::~YpqReader() = default;

	std::unique_ptr<Source> YpqReader::open(std::size_t index) const
	{
		if (index >= _entries.size())
			return {};
		const auto& entry = _entries[index];
		return Source::from(_source, entry._offset, entry._size);
	}

	struct YpqWriter::Entry
	{
		std::string _name;

		Entry(std::string_view name)
			: _name{ name } {}
	};

	YpqWriter::YpqWriter(Writer&& writer)
		: _writer(std::move(writer))
	{
	}

	YpqWriter::~YpqWriter()
	{
		if (!_finished)
			_writer.unlink();
	}

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

		const auto metadata_offset = sizeof(YpqHeader) + sizeof(YpqEntry) * _entries.size();

		Buffer metadata_buffer;
		{
			Writer writer{ metadata_buffer };

			const auto write_string = [&writer](const std::string& value) {
				const auto size = static_cast<std::uint8_t>(value.size());
				return size == value.size() && writer.write(size) && writer.write(value.data(), size);
			};

			for (const auto& entry : _entries)
			{
				entries.emplace_back().metadata_offset = static_cast<std::uint32_t>(metadata_offset + writer.offset());
				if (!write_string(entry._name))
					return false;
			}
		}

		const auto data_offset = metadata_offset + metadata_buffer.size();

		for (auto& entry : entries)
			entry.data_offset = data_offset;

		YpqHeader header;
		header.signature = YpqHeader::Signature;
		header.entry_count = static_cast<std::uint32_t>(_entries.size());
		header.index_size = static_cast<std::uint32_t>(data_offset);
		if (!_writer.write(header))
			return false;

		if (_writer.write(entries.data(), entries.size() * sizeof(YpqEntry)) != entries.size() * sizeof(YpqEntry))
			return false;

		if (!_writer.write_all(metadata_buffer))
			return false;

		for (const auto& entry : _entries)
		{
			const auto source = Source::from(entry._name);
			if (!source)
				return false;
			const auto i = static_cast<std::size_t>(&entry - _entries.data());
			entries[i].data_offset = decltype(entries[i].data_offset){ _writer.offset() };
			entries[i].data_size = static_cast<std::uint32_t>(source->size());
			if (entries[i].data_size != source->size())
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
