#ifndef __BASE_POSIX_FILE_H
#define __BASE_POSIX_FILE_H

#include <yttrium/file.h>
#include <yttrium/string.h>

#include "../private_base.h"

namespace Yttrium
{

class Y_PRIVATE File::Private: public PrivateBase<File::Private>
{
public:

	int      descriptor;
	unsigned mode;
	String   name;
	bool     auto_remove;

public:

	Private(Allocator* allocator)
		: PrivateBase(allocator)
		, descriptor(-1)
		, mode(0)
		, name(allocator)
		, auto_remove(false)
	{
	}

	~Private();

public:

	static int open(const StaticString& name, int flags, Allocator* allocator);
};

} // namespace Yttrium

#endif // __BASE_POSIX_FILES_H
