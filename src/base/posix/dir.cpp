#include "dir.h"

#include <Yttrium/assert.h>
#include <Yttrium/string.h>

#include <cstdio>

#include <errno.h>
#include <unistd.h>

namespace Yttrium
{

Dir::Private::Private(DIR *dir, const char *path, Allocator *allocator)
	: PrivateBase(allocator)
	, dir(dir)
{
	long max_name_size = -1;

	int dir_descriptor = ::dirfd(dir);
	if (Y_UNLIKELY(dir_descriptor < 0))
	{
		Y_ASSERT(errno == ENOTSUP);
		max_name_size = ::pathconf(path, _PC_NAME_MAX);
	}
	else
	{
		max_name_size = ::fpathconf(dir_descriptor, _PC_NAME_MAX);
	}
	
	if (max_name_size == -1)
	{
		max_name_size = FILENAME_MAX;
	}

	dirent_size = offsetof(::dirent, d_name) + max_name_size + 1;
}

Dir::Iterator::~Iterator()
{
	if (_private)
	{
		_private->allocator->deallocate(_private);
	}
}

void Dir::Iterator::operator ++()
{
	int result = ::readdir_r(_private->dir, &_private->dirent, &_private->self);
	Y_ASSERT(!result);
}

StaticString Dir::Iterator::operator *() const
{
	return _private->dirent.d_name;
}

bool Dir::Iterator::operator !=(Iterator iterator) const
{
	return (_private && _private->self) != (iterator._private && iterator._private->self);
}

Dir::Iterator Dir::begin() const
{
	if (Y_LIKELY(_private))
	{
		Iterator iterator(static_cast<Iterator::Private *>(
			_private->_allocator->allocate(offsetof(Iterator::Private, dirent) + _private->dirent_size)));
		iterator._private->allocator = _private->_allocator;
		if (!::readdir_r(_private->dir, &iterator._private->dirent, &iterator._private->self))
		{
			iterator._private->dir = _private->dir;
			return iterator;
		}
	}
	return end();
}

bool Dir::open(const StaticString &name, Allocator *allocator)
{
	close();

	const String &final_name = name.zero_terminated(allocator);

	DIR *dir = ::opendir(final_name.text());
	if (!dir)
	{
		return false;
	}

	_private = Y_NEW(allocator, Private)(dir, final_name.text(), allocator);

	return true;
}

} // namespace Yttrium
