#ifndef __BASE_POSIX_DIR_H
#define __BASE_POSIX_DIR_H

#include <yttrium/dir.h>

#include "../private_base.h"

#include <dirent.h>

namespace Yttrium
{

class Dir::Iterator::Private
{
public:

	Allocator *allocator;
	DIR       *dir;
	::dirent  *self;
	::dirent   dirent;
};

class Y_PRIVATE Dir::Private
	: public PrivateBase<Dir::Private>
{
public:

	DIR    *dir;
	size_t  dirent_size;

public:

	Private(DIR *dir, const char *path, Allocator *allocator);

	~Private()
	{
		::closedir(dir);
	}
};

} // namespace Yttrium

#endif // __BASE_POSIX_DIR_H
