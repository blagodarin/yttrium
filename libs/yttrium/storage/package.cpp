#include <yttrium/storage/package.h>

#include <yttrium/storage/source.h>
#include "formats/ypq.h"
#include "package.h"

#include <iostream>
#include <string>

namespace Yttrium
{
	std::unique_ptr<PackageReader> PackageReader::create(const std::string& path, PackageType type)
	{
		if (type == PackageType::Auto)
		{
			if (strings::ends_with(path, ".ypq"))
				type = PackageType::Ypq;
			else
				return {};
		}
		auto source = Source::from(path);
		if (!source)
			return {};
		try
		{
			if (type == PackageType::Ypq)
				return std::make_unique<YpqReader>(std::move(source));
		}
		catch (const BadPackage& e)
		{
			std::cerr << "(" << path << ") " << e.what() << "\n";
		}
		return {};
	}

	std::unique_ptr<PackageWriter> PackageWriter::create(const std::string& path, PackageType type)
	{
		if (type == PackageType::Auto)
		{
			if (strings::ends_with(path, ".ypq"))
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
