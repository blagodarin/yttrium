#include "ypq.h"

#include "../base/fourcc.h"

#include <cstring>
#include <limits>

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

		const uint64_t YpqPackageSignature = "\xDA\xBDYPQA\xED\xDE"_eightcc;
		const uint64_t YpqFileSignature    = "\xDA\xBDYPQF\xED\xDE"_eightcc;
		const uint64_t YpqIndexSignature   = "\xDA\xBDYPQI\xED\xDE"_eightcc;
	}

	YpqReader::YpqReader(File&& file, Allocator& allocator)
		: PackageReaderImpl(std::move(file), allocator)
	{
		YpqPackageHeader package_header;

		if (!_file.seek(sizeof(package_header), File::Reverse)
			|| !_file.read(&package_header) || package_header.signature != YpqPackageSignature)
		{
			throw BadPackage(String("(package/ypq) Bad package"_s, &allocator));
		}

		YpqFileHeader index_file_header;

		if (!_file.seek(package_header.index_file_offset)
			|| !_file.read(&index_file_header) || index_file_header.signature != YpqFileSignature)
		{
			throw BadPackage(String("(package/ypq) Bad package"_s, &allocator));
		}

		YpqIndexHeader index_header;

		if (!_file.read(&index_header) || index_header.signature != YpqIndexSignature
			|| index_file_header.size < sizeof(index_header))
		{
			throw BadPackage(String("(package/ypq) Bad package"_s, &allocator));
		}

		uint32_t index_size = index_file_header.size - sizeof(index_header);

		for (uint32_t i = 0; i < index_header.size; ++i)
		{
			YpqIndexEntry index_entry;

			if (index_size < sizeof(index_entry))
				throw BadPackage(String("(package/ypq) Bad package"_s, &allocator));

			index_size -= sizeof(index_entry);

			if (!_file.read(&index_entry) || index_size < index_entry.name_size)
				throw BadPackage(String("(package/ypq) Bad package"_s, &allocator));

			index_size -= index_entry.name_size;

			String name(index_entry.name_size, &_allocator);

			name.resize(index_entry.name_size);

			if (!_file.read(name.text(), index_entry.name_size))
				throw BadPackage(String("(package/ypq) Bad package"_s, &allocator));

			_index[name] = index_entry.offset;
		}

		if (index_size)
			throw BadPackage(String("(package/ypq) Bad package"_s, &allocator)); // Disallow index padding.
	}

	PackedFile YpqReader::do_open_file(const StaticString& name)
	{
		const auto i = _index.find(String(name, ByReference()));
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
			flush_file();

		_last_offset = _file.offset();

		YpqFileHeader index_file_header;

		index_file_header.signature = YpqFileSignature;
		index_file_header.size = 0;

		_file.write(index_file_header);

		YpqIndexHeader index_header;

		index_header.signature = YpqIndexSignature;
		index_header.size = _entries.size();

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
		package_header.index_file_offset = _last_offset;

		_file.write(package_header);
	}

	PackedFile YpqWriter::do_open_file(const StaticString& name)
	{
		YpqFileHeader file_header;

		if (!_entries.empty())
			flush_file();

		_last_offset = _file.offset();

		_entries.push_back(Entry(_last_offset, String(name, &_allocator)));

		file_header.signature = YpqFileSignature;
		file_header.size = 0;

		return _file.write(file_header) ? PackedFile(&_file) : PackedFile();
	}

	void YpqWriter::flush_file()
	{
		_file.seek(0, File::Reverse);

		const auto begin_offset = _last_offset + sizeof(YpqFileHeader);
		const auto end_offset = _file.offset();
		if (end_offset - begin_offset > std::numeric_limits<uint32_t>::max())
			throw std::logic_error("Unsupported file size");

		_file.seek(_last_offset + offsetof(YpqFileHeader, size));
		_file.write(static_cast<uint32_t>(end_offset - begin_offset));
		_file.seek(end_offset);
	}
}
