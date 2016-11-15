#include "ypq.h"

#include <yttrium/memory/buffer.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>
#include <yttrium/string_format.h>
#include "../../utils/fourcc.h"
#include "../reader.h"

#include <cstring>
#include <limits>

namespace
{
	using namespace Yttrium;

#pragma pack(push, 1)

	struct YpqPackageHeader
	{
		uint64_t signature = 0;
		uint64_t index_offset = 0;

		static constexpr auto Signature = "\xDA\xBDYPQA\xED\xDE"_eightcc;
	};

	struct YpqIndexHeader
	{
		uint64_t signature = 0;
		uint32_t size = 0;
		uint32_t entry_count = 0;

		static constexpr auto Signature = "\xC8\xCD\xCA\xD1\xD4\xC0\xC9\xCB"_eightcc;
	};

	struct YpqIndexEntry
	{
		uint64_t offset = 0;
		uint32_t size = 0;
		// Followed by name and properties.
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

	YpqReader::YpqReader(String&& name, Reader&& reader, Allocator& allocator)
		: _name(std::move(name))
		, _reader(std::move(reader))
		, _entries(allocator)
	{
		YpqPackageHeader package_header;
		if (_reader.size() < sizeof package_header
			|| !_reader.read_at(_reader.size() - sizeof package_header, package_header)
			|| package_header.signature != YpqPackageHeader::Signature)
			throw BadPackage(String("Bad package header"_s, &allocator));

		YpqIndexHeader index_header;
		if (!_reader.seek(package_header.index_offset)
			|| !_reader.read(index_header)
			|| index_header.signature != YpqIndexHeader::Signature)
			throw BadPackage(String("Bad package index header"_s, &allocator));

		{
			Buffer index_buffer(index_header.size);
			if (!_reader.read(index_buffer.data(), index_buffer.size()))
				throw BadPackage(String("Bad package index"_s, &allocator));
			_index_buffer = std::make_shared<const Buffer>(std::move(index_buffer));
		}

		Reader index_reader(std::make_shared<BufferReader>(_index_buffer, _name)); // TODO: Span reader.

		const auto read_uint8 = [&allocator, &index_reader]
		{
			uint8_t value = 0;
			if (!index_reader.read(value))
				throw BadPackage(String("Bad package index entry"_s, &allocator));
			return value;
		};

		const auto read_string = [this, &allocator, &index_reader, &read_uint8]
		{
			const auto size = read_uint8();
			if (!index_reader.skip(size))
				throw BadPackage(String("Bad package index entry"_s, &allocator));
			return StaticString(static_cast<const char*>(_index_buffer->data()) + index_reader.offset() - size, size);
		};

		const auto reader_size = _reader.size();
		for (uint32_t i = 0; i < index_header.entry_count; ++i)
		{
			YpqIndexEntry index_entry;
			if (!index_reader.read(index_entry))
				throw BadPackage(String("Unable to read package index entry #"_s, &allocator) << i);
			if (index_entry.size > reader_size || index_entry.offset > reader_size - index_entry.size)
				throw BadPackage(String("Bad package index entry #"_s, &allocator) << i);
			Entry entry;
			entry.offset = index_entry.offset;
			entry.size = index_entry.size;
			const auto name = read_string();
			entry.properties_begin = _properties.size();
			for (auto property_count = read_uint8(); property_count > 0; --property_count)
			{
				const auto property_name = read_string();
				const auto property_value = read_string();
				_properties.emplace_back(property_name, property_value);
			}
			entry.properties_end = _properties.size();
			_entries.emplace(name, entry);
		}

		if (index_reader.offset() != index_reader.size())
			throw BadPackage(String("Stray index data"_s, &allocator)); // Disallows index padding.
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
		uint64_t offset = 0;
		uint32_t size = 0;
		std::string name;
		std::map<std::string, std::string> properties;

		Entry(uint64_t offset, uint32_t size, std::string&& name, std::map<std::string, std::string>&& properties)
			: offset(offset), size(size), name(std::move(name)), properties(std::move(properties)) {}
	};

	YpqWriter::YpqWriter(Writer&& writer)
		: _writer(std::move(writer))
	{
	}

	YpqWriter::~YpqWriter()
	{
		if (!_committed)
			_writer.unlink();
	}

	bool YpqWriter::add(const StaticString& name, const Reader& reader, std::map<std::string, std::string>&& properties)
	{
		if (_committed || !reader)
			return false;
		const auto file_offset = _writer.size();
		const auto file_size = static_cast<uint32_t>(reader.size());
		if (file_size != reader.size() || !_writer.write_all(reader))
			return false;
		_entries.emplace_back(file_offset, file_size, std::string(name.text(), name.size()), std::move(properties));
		return true;
	}

	bool YpqWriter::commit()
	{
		if (_committed)
			return false;

		const auto write_string = [this](const std::string& value)
		{
			const auto size = static_cast<uint8_t>(value.size());
			return size == value.size() && _writer.write(size) && _writer.write(value.data(), size);
		};

		const auto index_offset = _writer.size();

		YpqIndexHeader index_header;
		index_header.signature = YpqIndexHeader::Signature;
		index_header.entry_count = _entries.size();
		if (!_writer.write(index_header))
			return false;

		for (const auto& entry : _entries)
		{
			YpqIndexEntry index_entry;
			index_entry.offset = entry.offset;
			index_entry.size = entry.size;
			if (!_writer.write(index_entry))
				return false;
			if (!write_string(entry.name))
				return false;
			const auto property_count = static_cast<uint8_t>(entry.properties.size());
			if (property_count != entry.properties.size() || !_writer.write(property_count))
				return false;
			for (const auto& property : entry.properties)
				if (!write_string(property.first) || !write_string(property.second))
					return false;
		}

		const auto index_size = static_cast<uint32_t>(_writer.size() - index_offset - sizeof index_header);
		if (!_writer.write_at(index_offset + offsetof(YpqIndexHeader, size), index_size))
			return false;

		YpqPackageHeader package_header;
		package_header.signature = YpqPackageHeader::Signature;
		package_header.index_offset = index_offset;
		if (!_writer.write_at(_writer.size(), package_header))
			return false;

		_committed = true;
		return true;
	}
}
