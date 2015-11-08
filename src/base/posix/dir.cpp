#include <yttrium/dir.h>

#include "../../utils/unique_ptr.h"
#include "../../utils/zero_terminated.h"
#include "../private_base.h"

#include <system_error>

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
		const auto result = ::readdir_r(_private->_dir, &_private->_dirent, &dirent);
		if (result != 0)
			throw std::system_error(errno, std::generic_category());
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

	using P_DIR = Y_UNIQUE_PTR(DIR, ::closedir);

	class Y_PRIVATE Dir::Private : public PrivateBase<Dir::Private>
	{
	public:

		Private(P_DIR&& dir, long max_name_size, Allocator* allocator)
			: PrivateBase(allocator)
			, _dir(std::move(dir))
			, _iterator_private_size(offsetof(Iterator::Private, _dirent)
				+ offsetof(::dirent, d_name) + max_name_size + 1)
		{
		}

	public:

		P_DIR _dir;
		const size_t _iterator_private_size;
	};

	Y_IMPLEMENT_UNIQUE(Dir);

	Dir::Dir(const StaticString& name, Allocator* allocator)
	{
		Y_ZERO_TERMINATED(name_z, name);

		P_DIR dir(::opendir(name_z));
		if (!dir)
			return;

		long max_name_size = -1;

		const auto dir_descriptor = ::dirfd(dir.get());
		if (dir_descriptor < 0)
		{
			assert(errno == ENOTSUP);
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

		_private = Y_NEW(allocator, Private)(std::move(dir), max_name_size, allocator);
	}

	Dir::Iterator Dir::begin() const
	{
		if (!_private)
			return end();

		Iterator iterator(new(_private->_allocator->allocate(_private->_iterator_private_size))
			Iterator::Private(_private->_dir.get(), _private->_allocator));
		::rewinddir(_private->_dir.get());
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
}
