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

	int      _descriptor;
	unsigned _mode;
	String   _name;
	bool     _auto_close;
	bool     _auto_remove;

public:

	Private(int descriptor, unsigned mode, Allocator* allocator);
	~Private();

public:

	static int open(const StaticString& name, int flags, Allocator* allocator);
};

} // namespace Yttrium

#endif // __BASE_POSIX_FILES_H
