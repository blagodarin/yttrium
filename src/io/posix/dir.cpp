#include <yttrium/io/dir.h>

#include "../../memory/raw.h"
#include "../../utils/unique_ptr.h"
#include "../../utils/zero_terminated.h"

#include <cassert>
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
		Private(Allocator& allocator, DIR* dir)
			: _allocator(&allocator)
			, _dir(dir)
		{
		}

	public:
		Allocator* const _allocator; // TODO: Why changing it to reference generates compiler warning on offsetof?
		DIR* const _dir;
		::dirent _dirent;
	};

	Dir::Iterator::~Iterator()
	{
		if (_private)
			unmake_raw(*_private->_allocator, _private);
	}

	void Dir::Iterator::operator++()
	{
		::dirent* dirent = nullptr;
		const auto result = ::readdir_r(_private->_dir, &_private->_dirent, &dirent);
		if (result != 0)
			throw std::system_error(errno, std::generic_category());
		else if (dirent)
			return;
		unmake_raw(*_private->_allocator, _private);
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

	class DirPrivate
	{
	public:
		DirPrivate(P_DIR&& dir, size_t iterator_private_size)
			: _dir(std::move(dir))
			, _iterator_private_size(iterator_private_size)
		{
		}

	public:
		P_DIR _dir;
		const size_t _iterator_private_size;
	};

	Dir::Dir(const StaticString& name, Allocator& allocator)
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

		_private = make_unique<DirPrivate>(allocator, std::move(dir),
			offsetof(Dir::Iterator::Private, _dirent) + offsetof(::dirent, d_name) + max_name_size + 1);
	}

	Dir::Iterator Dir::begin() const
	{
		if (!_private)
			return end();
		Iterator iterator(make_raw_sized<Iterator::Private>(_private.allocator(),
			_private->_iterator_private_size, _private.allocator(), _private->_dir.get()));
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

	Dir::~Dir() = default;
}
