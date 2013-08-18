#ifndef __BASE_POSIX_FILE_H
#define __BASE_POSIX_FILE_H

#include <yttrium/file.h>
#include <yttrium/string.h>

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

	Private(Allocator *allocator)
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
		, _private_data(nullptr)
	{
	}

	StaticFile(const StaticString &name, Mode mode, Allocator *allocator)
		//: StaticFile() // TODO: Uncomment.
		: File(&_private_data)
		, _private_data(nullptr)
	{
		File::open(name, mode, allocator);
	}

private:

	Private _private_data;
};

} // namespace Yttrium

#endif // __BASE_POSIX_FILES_H
