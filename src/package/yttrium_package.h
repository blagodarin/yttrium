#ifndef __FILE_SYSTEM_YTTRIUM_PACKAGE_H
#define __FILE_SYSTEM_YTTRIUM_PACKAGE_H

#include <Yttrium/package.h>

namespace Yttrium
{

class YttriumPackageReader: public PackageReader
{
public:

	static YttriumPackageReader *open(const StaticString &name);
};

class YttriumPackageWriter: public PackageWriter
{
public:

	static YttriumPackageWriter *open(const StaticString &name);
};

} // namespace Yttrium

#endif // __FILE_SYSTEM_YTTRIUM_PACKAGE_H
