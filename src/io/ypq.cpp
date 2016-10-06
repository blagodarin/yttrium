#include "ypq.h"

#include <yttrium/log.h>
#include "../base/fourcc.h"

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
	YpqReader::YpqReader(File&& file, Allocator& allocator)
		: _file(std::move(file))
		, _allocator(allocator)
		, _index(_allocator)
	{
		YpqPackageHeader package_header;
		if (_file.size() < sizeof package_header
			|| !_file.seek(_file.size() - sizeof package_header)
			|| !_file.read(&package_header)
			|| package_header.signature != YpqPackageSignature)
			throw BadPackage(String("Bad package header"_s, &_allocator));

		YpqFileHeader index_file_header;
		if (!_file.seek(package_header.index_file_offset)
			|| !_file.read(&index_file_header)
			|| index_file_header.signature != YpqFileSignature)
			throw BadPackage(String("Bad package index file header"_s, &_allocator));

		YpqIndexHeader index_header;
		if (!_file.read(&index_header)
			|| index_header.signature != YpqIndexSignature
			|| index_file_header.size < sizeof index_header)
			throw BadPackage(String("Bad package index header"_s, &_allocator));

		auto index_size = index_file_header.size - sizeof index_header;

		for (uint32_t i = 0; i < index_header.size; ++i)
		{
			YpqIndexEntry entry;
			if (index_size < sizeof entry
				|| !_file.read(&entry))
				throw BadPackage(String("Bad package index entry"_s, &_allocator));

			index_size -= sizeof entry;

			String name(entry.name_size, &_allocator);
			name.resize(entry.name_size);
			if (index_size < entry.name_size
				|| !_file.read(name.text(), entry.name_size))
				throw BadPackage(String("Bad package index entry name"_s, &_allocator));

			index_size -= entry.name_size;

			_index[name] = entry.offset;
		}

		if (index_size)
			throw BadPackage(String("Stray index data"_s, &_allocator)); // Disallow index padding.
	}

	File YpqReader::open_file(const StaticString& name)
	{
		const auto i = _index.find(String(name, ByReference()));
		if (i == _index.end())
			return {};

		YpqFileHeader file_header;
		if (!_file.seek(i->second)
			|| !_file.read(&file_header)
			|| file_header.signature != YpqFileSignature) // TODO: Check file size.
		{
			Log() << "("_s << _file.name() << ") Bad package"_s;
			return {};
		}

		File file;
		FilePrivate::set(file, make_unique<PackedFile>(_allocator,
			String(&NoAllocator), File::Read, file_header.size, FilePrivate::get(_file), i->second + sizeof file_header));
		return file;
	}

	YpqWriter::~YpqWriter()
	{
		if (!_entries.empty())
			flush_file();

		YpqFileHeader index_file_header;
		index_file_header.signature = YpqFileSignature;
		index_file_header.size = 0;

		YpqIndexHeader index_header;
		index_header.signature = YpqIndexSignature;
		index_header.size = _entries.size();

		_file.seek(_file.size());
		_file.write(index_file_header);

		_last_base = _file.size();

		_file.write(index_header);

		for (const auto& entry : _entries)
		{
			YpqIndexEntry index_entry;
			index_entry.offset = entry.offset;
			index_entry.name_size = entry.name.size();

			_file.write(index_entry);
			_file.write(entry.name.text(), entry.name.size());
		}

		flush_file();

		YpqPackageHeader package_header;
		package_header.signature = YpqPackageSignature;
		package_header.index_file_offset = _last_base - sizeof index_file_header;

		_file.seek(_file.size());
		_file.write(package_header);
	}

	File YpqWriter::open_file(const StaticString& name)
	{
		if (!_entries.empty())
			flush_file();

		YpqFileHeader file_header;
		file_header.signature = YpqFileSignature;
		file_header.size = 0;
		_file.seek(_file.size());
		if (!_file.write(file_header))
			return {};

		_last_base = _file.size();
		_entries.emplace_back(_last_base - sizeof file_header, String(name, &_allocator));

		File file;
		FilePrivate::set(file, make_unique<PackedFile>(_allocator,
			String(&NoAllocator), File::Write, 0, FilePrivate::get(_file), _last_base));
		return file;
	}

	void YpqWriter::flush_file()
	{
		const auto file_size = _file.size() - _last_base;
		if (file_size > std::numeric_limits<uint32_t>::max())
			throw std::logic_error("Unsupported file size");
		_file.seek(_last_base - sizeof(YpqFileHeader) + offsetof(YpqFileHeader, size));
		_file.write(static_cast<uint32_t>(file_size));
	}
}
