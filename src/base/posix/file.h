#ifndef __BASE_POSIX_FILE_H
#define __BASE_POSIX_FILE_H

#include <Yttrium/file.h>
#include <Yttrium/noncopyable.h>

#include "../private_base.h"

namespace Yttrium
{

class Y_PRIVATE File::Private: public PrivateBase<File::Private>
{
public:

	int  descriptor;
	Mode mode;

public:

	Private(int descriptor = -1, Mode mode = 0, Allocator *allocator = nullptr)
		: PrivateBase(allocator)
		, descriptor(descriptor)
		, mode(mode)
	{
	}

public:

	static int open(const StaticString &name, int flags, Allocator *allocator);
};

class StaticFile: public File, public Noncopyable
{
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
