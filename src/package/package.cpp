#include "package.h"

#include <yttrium/log.h>
#include "ypq.h"

namespace Yttrium
{
	Pointer<PackageReader> PackageReader::create(const StaticString& package, PackageType type, Allocator& allocator)
	{
		if (type == PackageType::Auto)
		{
			if (package.file_name_extension() == ".ypq"_s)
				type = PackageType::Ypq;
			else
				return {};
		}

		File file(package, File::Read, &allocator);
		if (!file)
			return {};

		try
		{
			switch (type)
			{
			case PackageType::Ypq: return make_pointer<YpqReader>(allocator, std::move(file), allocator);
			default: return {};
			}
		}
		catch (const BadPackage& e)
		{
			Log() << e.what();
			return {};
		}
	}

	File PackageReaderImpl::open_file(const StaticString& name)
	{
		const auto& packed_file = do_open_file(name);
		return packed_file.file ? File(packed_file.file->_private, packed_file.file->_offset, packed_file.size) : File();
	}

	Pointer<PackageWriter> PackageWriter::create(const StaticString& package, PackageType type, Allocator& allocator)
	{
		if (type == PackageType::Auto)
		{
			if (package.file_name_extension() == ".ypq"_s)
				type = PackageType::Ypq;
			else
				return {};
		}

		File file(package, File::Write | File::Truncate, &allocator);
		if (!file)
			return {};

		switch (type)
		{
		case PackageType::Ypq: return make_pointer<YpqWriter>(allocator, std::move(file), allocator);
		default: return {};
		}
	}

	File PackageWriterImpl::open_file(const StaticString& name)
	{
		const auto& packed_file = do_open_file(name);
		return packed_file.file ? File(packed_file.file->_private, packed_file.file->_offset, packed_file.size) : File();
	}
}
