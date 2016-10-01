#include "package.h"

#include <yttrium/log.h>
#include <yttrium/memory/unique_ptr.h>
#include "ypq.h"

#include <limits>

namespace Yttrium
{
	bool PackedFile::flush()
	{
		return _package->flush();
	}

	bool PackedFile::resize(uint64_t size)
	{
		if (size > std::numeric_limits<uint64_t>::max() - _base)
			return false;
		return _package->resize(_base + size);
	}

	size_t PackedFile::read(void*, size_t)
	{
		throw std::logic_error("Bad packed file operation");
	}

	size_t PackedFile::read(void* buffer, size_t size, uint64_t offset)
	{
		if (offset > std::numeric_limits<uint64_t>::max() - _base)
			return 0;
		return _package->read(buffer, size, _base + offset);
	}

	size_t PackedFile::write(const void*, size_t)
	{
		throw std::logic_error("Bad packed file operation");
	}

	size_t PackedFile::write(const void* buffer, size_t size, uint64_t offset)
	{
		if (offset > std::numeric_limits<uint64_t>::max() - _base)
			return 0;
		const auto result = _package->write(buffer, size, _base + offset);
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
