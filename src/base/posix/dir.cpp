#include <yttrium/dir.h>

#include <yttrium/allocator.h>
#include <yttrium/assert.h>
#include <yttrium/string.h>
#include "../private_base.h"

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Yttrium
{

class Dir::Iterator::Private
{
public:

	Allocator* _allocator;
	DIR*       _dir;
	::dirent   _dirent;

	Private(DIR* dir, Allocator* allocator)
		: _allocator(allocator)
		, _dir(dir)
	{
	}
};

Dir::Iterator::~Iterator()
{
	if (_private)
		_private->_allocator->deallocate(_private);
}

void Dir::Iterator::operator++()
{
	::dirent* dirent = nullptr;
	int result = ::readdir_r(_private->_dir, &_private->_dirent, &dirent);
	if (result != 0)
		Y_ASSERT(false); // TODO: Report the error in a better way.
	else if (dirent)
		return;
	_private->_allocator->deallocate(_private);
	_private = nullptr;
}

StaticString Dir::Iterator::operator*() const
{
	return _private->_dirent.d_name;
}

bool Dir::Iterator::operator!=(Iterator iterator) const
{
	return _private != iterator._private;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Dir::Private: public PrivateBase<Dir::Private>
{
public:

	Private(DIR* dir, long max_name_size, Allocator* allocator)
		: PrivateBase(allocator)
		, _dir(dir)
		, _iterator_private_size(offsetof(Iterator::Private, _dirent)
			+ offsetof(::dirent, d_name) + max_name_size + 1)
	{
	}

	~Private()
	{
		::closedir(_dir);
	}

public:

	DIR*   _dir;
	size_t _iterator_private_size;
};

Y_IMPLEMENT_PRIVATE_NONCOPYABLE(Dir);

Dir::Dir(const StaticString& name, Allocator* allocator)
	: Dir()
{
	Y_ZERO_TERMINATED(name_z, name);

	const auto dir = ::opendir(name_z);
	if (!dir)
		return;

	long max_name_size = -1;

	const auto dir_descriptor = ::dirfd(dir);
	if (dir_descriptor < 0)
	{
		Y_ASSERT(errno == ENOTSUP);
		max_name_size = ::pathconf(name_z, _PC_NAME_MAX);
	}
	else
	{
		max_name_size = ::fpathconf(dir_descriptor, _PC_NAME_MAX);
	}

	if (max_name_size == -1)
	{
		// _POSIX_NAME_MAX is 14, too small.
		// FILENAME_MAX is 4096, too large.
		max_name_size = 255;
	}

	_private = Y_NEW(allocator, Private)(dir, max_name_size, allocator);
}

Dir::Iterator Dir::begin() const
{
	if (!_private)
		return end();

	Iterator iterator(new(_private->_allocator->allocate(_private->_iterator_private_size))
		Iterator::Private(_private->_dir, _private->_allocator));
	::rewinddir(_private->_dir);
	++iterator;
	return iterator;
}

bool Dir::exists(const StaticString& name)
{
	Y_ZERO_TERMINATED(name_z, name);

	struct stat stat_buffer;
	if (::stat(name_z, &stat_buffer))
		return false;

	return S_ISDIR(stat_buffer.st_mode);
}

} // namespace Yttrium
