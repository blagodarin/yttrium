#ifndef __PACKAGE_PACKAGE_H
#define __PACKAGE_PACKAGE_H

#include <Yttrium/package.h>

#include "../base/file.h"

namespace Yttrium
{

class Y_PRIVATE PackageReader::Private: public PrivateBase<PackageReader::Private>
{
public:

	Private(Allocator *allocator)
		: PrivateBase(allocator)
	{
	}

	virtual ~Private()
	{
	}

public:

	virtual bool open() = 0;

public:

	StaticFile _file;
};

class Y_PRIVATE PackageWriter::Private: public PrivateBase<PackageWriter::Private>
{
public:

	Private(Allocator *allocator)
		: PrivateBase(allocator)
	{
	}

	virtual ~Private()
	{
	}

public:

	virtual bool open();

public:

	StaticFile _file;
};

} // namespace Yttrium

#endif // __PACKAGE_PACKAGE_H
