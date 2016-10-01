#include "package.h"

#include <yttrium/log.h>
#include <yttrium/memory/unique_ptr.h>
#include "ypq.h"

#include <cstdlib>

namespace Yttrium
{
	bool PackedFile::flush()
	{
		return _package->flush();
	}

	bool PackedFile::resize(uint64_t size)
	{
		return _package->resize(_base + size); // TODO: Prevent overflow.
	}

	size_t PackedFile::read(void*, size_t)
	{
		std::abort();
	}

	size_t PackedFile::read(void* buffer, size_t size, uint64_t offset)
	{
		return _package->read(buffer, size, _base + offset); // TODO: Prevent overflow.
	}

	size_t PackedFile::write(const void*, size_t)
	{
		std::abort();
	}

	size_t PackedFile::write(const void* buffer, size_t size, uint64_t offset)
	{
		const auto result = _package->write(buffer, size, _base + offset); // TODO: Prevent overflow.
		_package->update_size(_base + offset + result);
		return result;
	}

	UniquePtr<PackageReader> PackageReader::create(const StaticString& path, PackageType type, Allocator& allocator)
	{
		if (type == PackageType::Auto)
		{
			if (path.ends_with(".ypq"_s))
				type = PackageType::Ypq;
			else
				return {};
		}
		File file(path, File::Read, allocator);
		if (!file)
			return {};
		try
		{
			switch (type)
			{
			case PackageType::Ypq: return make_unique<YpqReader>(allocator, std::move(file), allocator);
			default: break;
			}
		}
		catch (const BadPackage& e)
		{
			Log() << "("_s << path << ") "_s << e.what();
		}
		return {};
	}

	UniquePtr<PackageWriter> PackageWriter::create(const StaticString& path, PackageType type, Allocator& allocator)
	{
		if (type == PackageType::Auto)
		{
			if (path.ends_with(".ypq"_s))
				type = PackageType::Ypq;
			else
				return {};
		}
		File file(path, File::Write | File::Truncate, allocator);
		if (!file)
			return {};
		switch (type)
		{
		case PackageType::Ypq: return make_unique<YpqWriter>(allocator, std::move(file), allocator);
		default: break;
		}
		return {};
	}
}
