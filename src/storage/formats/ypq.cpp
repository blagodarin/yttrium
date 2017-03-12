#include "ypq.h"

#include <yttrium/static_string.h>
#include "../../utils/fourcc.h"
#include "../package.h"

#include <cstring>
#include <limits>

namespace
{
	using namespace Yttrium;

#pragma pack(push, 1)

	struct YpqHeader
	{
		uint32_t signature = 0;
		uint32_t entry_count = 0;
		uint32_t index_size = 0;
		uint32_t reserved = 0;

		static constexpr auto Signature = "\xDFYPQ"_fourcc;
	};

	struct YpqEntry
	{
		uint64_t data_offset = 0;
		uint32_t data_size = 0;
		uint32_t metadata_offset = 0; // Name and properties.
	};

#pragma pack(pop)
}

namespace Yttrium
{
	struct YpqReader::Entry
	{
		uint64_t offset = 0;
		uint32_t size = 0;
		size_t properties_begin = 0;
		size_t properties_end = 0;
	};

	YpqReader::YpqReader(Reader&& reader)
		: _reader(std::move(reader))
	{
		YpqHeader header;
		if (!_reader.read_at(0, header)
			|| header.signature != YpqHeader::Signature)
			throw BadPackage("Not an YPQ package"_s);

		const auto metadata_offset = sizeof header + sizeof(YpqEntry) * header.entry_count;
		if (header.index_size < metadata_offset)
			throw BadPackage("Bad package header"_s);

		std::vector<YpqEntry> entries(size_t{ header.entry_count });
		if (!_reader.read_all_at(sizeof header, entries.data(), entries.size() * sizeof(YpqEntry)))
			throw BadPackage("Bad package index"_s);

		_metadata_buffer.reset(size_t{ header.index_size - metadata_offset });
		if (!_reader.read_all_at(metadata_offset, _metadata_buffer.data(), _metadata_buffer.size()))
			throw BadPackage("Bad package metadata"_s);

		Reader metadata_reader(_metadata_buffer.data(), _metadata_buffer.size());

		const auto read_uint8 = [&metadata_reader]
		{
			uint8_t value = 0;
			if (!metadata_reader.read(value))
				throw BadPackage("Bad package index entry metadata"_s);
			return value;
		};

		const auto read_string = [this, &metadata_reader, &read_uint8]
		{
			const auto size = read_uint8();
			if (!metadata_reader.skip(size))
				throw BadPackage("Bad package index entry metadata"_s);
			return StaticString(static_cast<const char*>(_metadata_buffer.data()) + metadata_reader.offset() - size, size);
		};

		const auto reader_size = _reader.size();
		for (const auto& entry : entries)
		{
			const auto i = &entry - entries.data();
			if (entry.data_offset > reader_size || entry.data_offset + entry.data_size > reader_size)
				throw BadPackage("Bad package index entry #"_s, i, " data"_s);
			if (entry.metadata_offset < metadata_offset || !metadata_reader.seek(entry.metadata_offset - metadata_offset))
				throw BadPackage("Bad package index entry #"_s, i, " metadata"_s);
			Entry internal;
			internal.offset = entry.data_offset;
			internal.size = entry.data_size;
			const auto name = read_string();
			internal.properties_begin = _properties.size();
			for (auto property_count = read_uint8(); property_count > 0; --property_count)
			{
				const auto property_name = read_string();
				const auto property_value = read_string();
				_properties.emplace_back(property_name, property_value);
			}
			internal.properties_end = _properties.size();
			_entries.emplace(name, internal);
		}
	}

	YpqReader::~YpqReader() = default;

	Reader YpqReader::open(const StaticString& name) const
	{
		const auto i = _entries.find(name);
		if (i == _entries.end())
			return {};
		Reader reader(_reader, i->second.offset, i->second.size);
		for (size_t j = i->second.properties_begin; j < i->second.properties_end; ++j)
			reader.set_property(_properties[j].first, _properties[j].second);
		return reader;
	}

	struct YpqWriter::Entry
	{
		std::string name;
		std::map<std::string, std::string> properties;
		uint64_t offset = 0;
		uint32_t size = 0;

		Entry(const std::string& name_, std::map<std::string, std::string>&& properties_)
			: name(name_), properties(std::move(properties_)) {}
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

	bool YpqWriter::add(const std::string& path, std::map<std::string, std::string>&& properties)
	{
		if (_committed)
			return false;
		_entries.emplace_back(path, std::move(properties));
		return true;
	}

	bool YpqWriter::commit()
	{
		if (_committed)
			return false;
		_committed = true;

		std::vector<YpqEntry> entries;

		const auto metadata_offset = sizeof(YpqHeader) + sizeof(YpqEntry) * _entries.size();

		Buffer metadata_buffer;
		{
			Writer writer(metadata_buffer);

			const auto write_string = [this, &writer](const std::string& value)
			{
				const auto size = static_cast<uint8_t>(value.size());
				return size == value.size() && writer.write(size) && writer.write(value.data(), size);
			};

			for (const auto& entry : _entries)
			{
				entries.emplace_back();
				entries.back().metadata_offset = metadata_offset + writer.offset();
				if (!write_string(entry.name))
					return false;
				const auto property_count = static_cast<uint8_t>(entry.properties.size());
				if (property_count != entry.properties.size() || !writer.write(property_count))
					return false;
				for (const auto& property : entry.properties)
					if (!write_string(property.first) || !write_string(property.second))
						return false;
			}
		}

		const auto data_offset = metadata_offset + metadata_buffer.size();

		for (auto& entry : entries)
			entry.data_offset = data_offset;

		YpqHeader header;
		header.signature = YpqHeader::Signature;
		header.entry_count = _entries.size();
		header.index_size = data_offset;
		if (!_writer.write(header))
			return false;

		if (_writer.write(entries.data(), entries.size() * sizeof(YpqEntry)) != entries.size() * sizeof(YpqEntry))
			return false;

		if (!_writer.write_all(metadata_buffer))
			return false;

		for (const auto& entry : _entries)
		{
			Reader reader(entry.name);
			if (!reader)
				return false;
			const auto i = static_cast<size_t>(&entry - _entries.data());
			entries[i].data_offset = decltype(entries[i].data_offset){ _writer.offset() };
			entries[i].data_size = reader.size();
			if (entries[i].data_size != reader.size())
				return false;
			if (!_writer.write_all(reader))
				return false;
		}

		_writer.seek(sizeof(YpqHeader));
		if (_writer.write(entries.data(), entries.size() * sizeof(YpqEntry)) != entries.size() * sizeof(YpqEntry))
			return false;

		_finished = true;
		return true;
	}
}
