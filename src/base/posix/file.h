#ifndef __BASE_POSIX_FILE_H
#define __BASE_POSIX_FILE_H

#include <Yttrium/file.h>
#include <Yttrium/string.h>

#include "../private_base.h"

namespace Yttrium
{

class Y_PRIVATE File::Private
	: public PrivateBase<File::Private>
{
public:

	int    descriptor;
	Mode   mode;
	String name;
	bool   auto_remove;

public:

	Private(Allocator *allocator = nullptr)
		: PrivateBase(allocator)
		, descriptor(-1)
		, mode(0)
		, name(allocator)
		, auto_remove(false)
	{
	}

	~Private();

public:

	static int open(const StaticString &name, int flags, Allocator *allocator);
};

class StaticFile
	: public File
{
	Y_NONCOPYABLE(StaticFile);

public:

	StaticFile()
		: File(&_private_data)
	{
	}

	StaticFile(const StaticString &name, Mode mode, Allocator *allocator)
		: File(&_private_data)
	{
		File::open(name, mode, allocator);
	}

private:

	Private _private_data;
};

} // namespace Yttrium

#endif // __BASE_POSIX_FILES_H
