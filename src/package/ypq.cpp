#include "ypq.h"

#include <cstring> // memcmp

#include "../base/fourcc.h"

namespace Yttrium
{

namespace
{

#pragma pack(push, 1)

struct YpqPackageHeader
{
	uint64_t signature;
	uint64_t index_file_offset;
};

struct YpqIndexHeader
{
	uint64_t signature;
	uint32_t size;
};

struct YpqIndexEntry
{
	uint64_t offset;
	uint8_t  name_size;
};

struct YpqFileHeader
{
	uint64_t signature;
	uint32_t size;
};

#pragma pack(pop)

// NOTE: It should be stated explicitly that the signatures are little endian.

const uint64_t YpqPackageSignature = Eightcc<0xDA, 0xBD, 'Y', 'P', 'Q', 'A', 0xED, 0xDE>::value;
const uint64_t YpqFileSignature    = Eightcc<0xDA, 0xBD, 'Y', 'P', 'Q', 'F', 0xED, 0xDE>::value;
const uint64_t YpqIndexSignature   = Eightcc<0xDA, 0xBD, 'Y', 'P', 'Q', 'I', 0xED, 0xDE>::value;

} // namespace

bool YpqReader::open()
{
	YpqPackageHeader package_header;

	if (!_file.seek(sizeof(package_header), File::Reverse)
		|| !_file.read(&package_header) || package_header.signature != YpqPackageSignature)
	{
		return false;
	}

	YpqFileHeader index_file_header;

	if (!_file.seek(package_header.index_file_offset)
		|| !_file.read(&index_file_header) || index_file_header.signature != YpqFileSignature)
	{
		return false;
	}

	YpqIndexHeader index_header;
	YpqIndexEntry  index_entry;

	if (!_file.read(&index_header) || index_header.signature != YpqIndexSignature
		|| (index_file_header.size - sizeof(index_header)) / sizeof(index_entry) < index_header.size) // TODO: Look for overflows.
	{
		return false;
	}

	uint32_t index_size = 0;

	for (uint32_t i = 0; i < index_header.size; ++i)
	{
		if (!_file.read(&index_entry))
		{
			return false;
		}

		String name(index_entry.name_size, _allocator);

		name.resize(index_entry.name_size);

		if (!_file.read(name.text(), index_entry.name_size))
		{
			return false;
		}

		_index[name] = index_entry.offset;

		index_size += sizeof(index_entry) + index_entry.name_size;
	}

	if (index_size != sizeof(index_entry) * index_header.size) // TODO: Look for overflows.
	{
		return false;
	}

	return true;
}

PackedFile YpqReader::open_file(const StaticString &name)
{
	Index::const_iterator i = _index.find(String(name, String::Ref));

	if (i != _index.end())
	{
		YpqFileHeader file_header;

		if (_file.seek(i->second) && _file.read(&file_header)
			&& file_header.signature == YpqFileSignature) // TODO: Check file size.
		{
			return PackedFile(&_file, file_header.size);
		}
	}

	return PackedFile();
}

YpqWriter::~YpqWriter()
{
	if (!_entries.empty())
	{
		flush_file();
	}

	_last_offset = _file.offset();

	YpqFileHeader index_file_header;

	index_file_header.signature = YpqFileSignature;
	index_file_header.size = 0;

	_file.write(index_file_header);

	YpqIndexHeader index_header;

	index_header.signature = YpqIndexSignature;
	index_header.size = _entries.size();

	_file.write(index_header);

	for (Entries::iterator i = _entries.begin(); i != _entries.end(); ++i)
	{
		YpqIndexEntry index_entry;

		index_entry.offset = i->offset;
		index_entry.name_size = i->name.size();

		_file.write(index_entry);
		_file.write(i->name.text(), i->name.size());
	}

	flush_file();

	YpqPackageHeader package_header;

	package_header.signature = YpqPackageSignature;
	package_header.index_file_offset = _last_offset;

	_file.write(package_header);

	_file.truncate();
}

PackedFile YpqWriter::open_file(const StaticString &name)
{
	YpqFileHeader file_header;

	if (!_entries.empty())
	{
		flush_file();
	}

	_last_offset = _file.offset();

	_entries.push_back(Entry(_last_offset, name));

	file_header.signature = YpqFileSignature;
	file_header.size = 0;

	if (_file.write(file_header))
	{
		return PackedFile(&_file);
	}

	return PackedFile();
}

void YpqWriter::flush_file()
{
	_file.seek(0, File::Reverse);

	UOffset offset = _file.offset();

	_file.seek(_last_offset + offsetof(YpqFileHeader, size));

	UOffset size = offset - _file.offset();

	// TODO: Check packed file size for uint32_t overflow.

	_file.write(static_cast<uint32_t>(size));

	_file.seek(offset);
}

} // namespace Yttrium
