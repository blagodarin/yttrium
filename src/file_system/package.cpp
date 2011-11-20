#include <Yttrium/package.hpp>

#include "yttrium_package.hpp"

namespace Yttrium
{

PackageReader *PackageReader::open(const StaticString &name, PackageFormat format)
{
	switch (format)
	{
	case PackageFormat::Yttrium: return YttriumPackageReader::open(name);

	default:

		// We should try all the formats here, but as we only have one...

		return YttriumPackageReader::open(name);
	}
}

PackageWriter *PackageWriter::open(const StaticString &name, PackageFormat format)
{
	switch (format)
	{
	case PackageFormat::Yttrium: return YttriumPackageWriter::open(name);
	default:                     return nullptr;
	}
}

} // namespace Yttrium
