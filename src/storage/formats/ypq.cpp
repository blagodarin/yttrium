#include "ypq.h"

#include <yttrium/log.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/writer.h>
#include "../../utils/fourcc.h"
#include "../reader.h"

#include <cstring>
#include <limits>

namespace
{
#pragma pack(push, 1)

	struct YpqPackageHeader
	{
		uint64_t signature = 0;
		uint64_t index_file_offset = 0;
	};

	struct YpqIndexHeader
	{
		uint32_t signature = 0;
		uint32_t entry_count = 0;
	};

	struct YpqIndexEntry
	{
		uint64_t offset = 0;
		// Followed by name and properties.
	};

	struct YpqFileHeader
	{
		uint32_t signature = 0;
		uint32_t size = 0;
	};

#pragma pack(pop)

	using namespace Yttrium;
	const auto YpqPackageSignature = "\xDA\xBDYPQA\xED\xDE"_eightcc;
	const auto YpqFileSignature = "\xD4\xC0\xC9\xCB"_fourcc;
	const auto YpqIndexSignature = "\xC8\xCD\xCA\xD1"_fourcc;
}

namespace Yttrium
{
	struct YpqReader::Entry
	{
		uint64_t offset = 0;
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
			|| package_header.signature != YpqPackageSignature)
			throw BadPackage(String("Bad package header"_s, &allocator));

		YpqFileHeader index_file_header;
		if (!_reader.seek(package_header.index_file_offset)
			||!_reader.read(index_file_header)
			|| index_file_header.signature != YpqFileSignature)
			throw BadPackage(String("Bad package index file header"_s, &allocator));

		{
			Buffer index_buffer(index_file_header.size);
			if (!_reader.read(index_buffer.data(), index_buffer.size()))
				throw BadPackage(String("Bad package index"_s, &allocator));
			_index_buffer = std::make_shared<const Buffer>(std::move(index_buffer));
		}

		Reader index_reader(std::make_shared<BufferReader>(_index_buffer, _name));

		YpqIndexHeader index_header;
		if (!index_reader.read(index_header)
			|| index_header.signature != YpqIndexSignature)
			throw BadPackage(String("Bad package index header"_s, &allocator));

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

		for (uint32_t i = 0; i < index_header.entry_count; ++i)
		{
			Entry entry;
			if (!index_reader.read(entry.offset))
				throw BadPackage(String("Unable to read package index entry #"_s, &allocator) << i);
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

		YpqFileHeader file_header;
		if (!_reader.read_at(i->second.offset, file_header)
			|| file_header.signature != YpqFileSignature) // TODO: Check file size.
		{
			Log() << "("_s << _name << ") Bad package"_s;
			return {};
		}

		// TODO: Add properties.
		return Reader(_reader, i->second.offset + sizeof file_header, file_header.size);
	}

	struct YpqWriter::Entry
	{
		uint64_t offset = 0;
		uint32_t size = 0;
		std::string name;
		std::map<std::string, std::string> properties;

		Entry(uint64_t offset, std::string&& name, std::map<std::string, std::string>&& properties)
			: offset(offset), name(std::move(name)), properties(std::move(properties)) {}
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
		YpqFileHeader file_header;
		file_header.signature = YpqFileSignature;
		file_header.size = reader.size();
		if (file_header.size != reader.size() || !_writer.write(file_header) || !_writer.write_all(reader))
			return false;
		_entries.emplace_back(file_offset, std::string(name.text(), name.size()), std::move(properties));
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

		const auto index_file_offset = _writer.size();

		YpqFileHeader index_file_header;
		index_file_header.signature = YpqFileSignature;
		index_file_header.size = 0;
		if (!_writer.write(index_file_header))
			return false;

		YpqIndexHeader index_header;
		index_header.signature = YpqIndexSignature;
		index_header.entry_count = _entries.size();
		if (!_writer.write(index_header))
			return false;

		for (const auto& entry : _entries)
		{
			YpqIndexEntry index_entry;
			index_entry.offset = entry.offset;
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

		const auto index_file_size = static_cast<uint32_t>(_writer.size() - index_file_offset - sizeof index_file_header);
		if (!_writer.write_at(index_file_offset + offsetof(YpqFileHeader, size), index_file_size))
			return false;

		YpqPackageHeader package_header;
		package_header.signature = YpqPackageSignature;
		package_header.index_file_offset = index_file_offset;
		if (!_writer.write_at(_writer.size(), package_header))
			return false;

		_committed = true;
		return true;
	}
}
