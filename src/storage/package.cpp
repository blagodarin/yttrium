#include <yttrium/storage/package.h>

#include "formats/ypq.h"
#include "package.h"

#include <iostream>

namespace Yttrium
{
	std::unique_ptr<PackageReader> PackageReader::create(const StaticString& path, PackageType type)
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
			if (type == PackageType::Ypq)
				return std::make_unique<YpqReader>(path.to_std(), std::move(reader));
		}
		catch (const BadPackage& e)
		{
			std::cerr << "(" << path << ") " << e.what() << "\n";
		}
		return {};
	}

	std::unique_ptr<PackageWriter> PackageWriter::create(const StaticString& path, PackageType type)
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
		if (type == PackageType::Ypq)
			return std::make_unique<YpqWriter>(std::move(writer));
		return {};
	}
}
