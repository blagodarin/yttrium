#include "ypq.h"

#include <yttrium/io/reader.h>
#include <yttrium/io/writer.h>
#include <yttrium/log.h>
#include "../../utils/fourcc.h"

#include <cstring>
#include <limits>

namespace
{
	using namespace Yttrium;

#pragma pack(push, 1)

	struct YpqPackageHeader
	{
		uint64_t signature = 0;
		uint64_t index_file_offset = 0;
	};

	struct YpqIndexHeader
	{
		uint64_t signature = 0;
		uint32_t size = 0;
	};

	struct YpqIndexEntry
	{
		uint64_t offset = 0;
		uint8_t  name_size = 0;
	};

	struct YpqFileHeader
	{
		uint64_t signature = 0;
		uint32_t size = 0;
	};

#pragma pack(pop)

	const auto YpqPackageSignature = "\xDA\xBDYPQA\xED\xDE"_eightcc;
	const auto YpqFileSignature = "\xDA\xBDYPQF\xED\xDE"_eightcc;
	const auto YpqIndexSignature = "\xDA\xBDYPQI\xED\xDE"_eightcc;
}

namespace Yttrium
{
	YpqReader::YpqReader(String&& name, Reader&& reader, Allocator& allocator)
		: _name(std::move(name))
		, _reader(std::move(reader))
		, _allocator(allocator)
		, _index(_allocator)
	{
		YpqPackageHeader package_header;
		if (_reader.size() < sizeof package_header
			|| !_reader.read_at(_reader.size() - sizeof package_header, package_header)
			|| package_header.signature != YpqPackageSignature)
			throw BadPackage(String("Bad package header"_s, &_allocator));

		YpqFileHeader index_file_header;
		if (!_reader.seek(package_header.index_file_offset)
			||!_reader.read(index_file_header)
			|| index_file_header.signature != YpqFileSignature)
			throw BadPackage(String("Bad package index file header"_s, &_allocator));

		YpqIndexHeader index_header;
		if (!_reader.read(index_header)
			|| index_header.signature != YpqIndexSignature
			|| index_file_header.size < sizeof index_header)
			throw BadPackage(String("Bad package index header"_s, &_allocator));

		auto index_size = index_file_header.size - sizeof index_header;

		for (uint32_t i = 0; i < index_header.size; ++i)
		{
			YpqIndexEntry entry;
			if (index_size < sizeof entry
				|| !_reader.read(entry))
				throw BadPackage(String("Bad package index entry"_s, &_allocator));

			index_size -= sizeof entry;

			String name(entry.name_size, &_allocator);
			name.resize(entry.name_size);
			if (index_size < entry.name_size
				|| _reader.read(name.text(), entry.name_size) != entry.name_size)
				throw BadPackage(String("Bad package index entry name"_s, &_allocator));

			index_size -= entry.name_size;

			_index[name] = entry.offset;
		}

		if (index_size)
			throw BadPackage(String("Stray index data"_s, &_allocator)); // Disallow index padding.
	}

	Reader YpqReader::open(const StaticString& name) const
	{
		const auto i = _index.find(String(name, ByReference()));
		if (i == _index.end())
			return {};

		YpqFileHeader file_header;
		if (!_reader.read_at(i->second, file_header)
			|| file_header.signature != YpqFileSignature) // TODO: Check file size.
		{
			Log() << "("_s << _name << ") Bad package"_s;
			return {};
		}

		return Reader(_reader, i->second + sizeof file_header, file_header.size);
	}

	YpqWriter::~YpqWriter()
	{
		const auto index_file_offset = _writer.size();

		YpqFileHeader index_file_header;
		index_file_header.signature = YpqFileSignature;
		index_file_header.size = 0;
		if (!_writer.write(index_file_header))
			return;

		YpqIndexHeader index_header;
		index_header.signature = YpqIndexSignature;
		index_header.size = _entries.size();
		if (!_writer.write(index_header))
			return;

		for (const auto& entry : _entries)
		{
			YpqIndexEntry index_entry;
			index_entry.offset = entry.offset;
			index_entry.name_size = entry.name.size();
			if (!_writer.write(index_entry) || !_writer.write_all(entry.name))
				return;
		}

		const auto index_file_size = static_cast<uint32_t>(_writer.size() - index_file_offset - sizeof index_file_header);
		if (!_writer.write_at(index_file_offset + offsetof(YpqFileHeader, size), index_file_size))
			return;

		YpqPackageHeader package_header;
		package_header.signature = YpqPackageSignature;
		package_header.index_file_offset = index_file_offset;
		_writer.write_at(_writer.size(), package_header);
	}

	bool YpqWriter::add(const StaticString& name, const Reader& reader)
	{
		if (!reader || reader.size() > std::numeric_limits<uint32_t>::max())
			return false;
		const auto file_offset = _writer.size();
		YpqFileHeader file_header;
		file_header.signature = YpqFileSignature;
		file_header.size = reader.size();
		if (!_writer.write(file_header) || !_writer.write_all(reader))
		{
			_writer.resize(file_offset);
			return false;
		}
		_entries.emplace_back(file_offset, String(name, &_allocator));
		return true;
	}

	void YpqWriter::unlink()
	{
		_writer.unlink();
		_writer = {};
	}
}
