#include "package.h"

#include <yttrium/log.h>
#include <yttrium/memory/unique_ptr.h>
#include "formats/ypq.h"

namespace Yttrium
{
	UniquePtr<PackageReader> PackageReader::create(const StaticString& path, PackageType type, Allocator& allocator)
	{
		if (type == PackageType::Auto)
		{
			if (path.ends_with(".ypq"_s))
				type = PackageType::Ypq;
			else
				return {};
		}
		Reader reader(path);
		if (!reader)
			return {};
		try
		{
			switch (type)
			{
			case PackageType::Ypq: return make_unique<YpqReader>(allocator, String(path, &allocator), std::move(reader), allocator);
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
		Writer writer(path);
		if (!writer)
			return {};
		switch (type)
		{
		case PackageType::Ypq: return make_unique<YpqWriter>(allocator, std::move(writer), allocator);
		default: break;
		}
		return {};
	}
}
