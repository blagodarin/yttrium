#ifndef __PACKAGE_PACKAGE_H
#define __PACKAGE_PACKAGE_H

#include <yttrium/package.h>

#include "../base/file.h"

namespace Yttrium
{

struct PackedFile
{
	File    *file;
	UOffset  size;

	PackedFile(File *file = nullptr, UOffset size = 0)
		: file(file)
		, size(size)
	{
	}
};

class Y_PRIVATE PackageReader::Private
	: public PrivateBase<PackageReader::Private>
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

	virtual PackedFile open_file(const StaticString &name) = 0;

public:

	File _file;
};

class Y_PRIVATE PackageWriter::Private
	: public PrivateBase<PackageWriter::Private>
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

	virtual bool open(PackageWriter::Mode mode);

	virtual PackedFile open_file(const StaticString &name) = 0;

public:

	File _file;
};

} // namespace Yttrium

#endif // __PACKAGE_PACKAGE_H
