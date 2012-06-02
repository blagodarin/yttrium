#include "file.hpp"

#include <Yttrium/package.hpp>
#include <Yttrium/string.hpp>

namespace Yttrium
{

File::File(const File &file)
	: _private(Private::copy(file._private))
	, _offset(file._offset)
	, _size(file._size)
	, _base(file._base)
{
}

bool File::is_opened() const
{
	return (_private && (_private->mode & ReadWrite));
}

bool File::open(const StaticString &name, Allocator *allocator)
{
	PackageManager *package_manager = PackageManager::instance();

	if (!package_manager)
	{
		return open(name, File::Read, allocator);
	}

	*this = package_manager->open_file(name);

	return (_private && (_private->mode & Read));
}

bool File::read_all(String *string)
{
	bool result = false;

	if (_private && (_private->mode & Read))
	{
		// We should unconditionally crash if the file is too large.
		// This way we shall have the same behavior for both files larger
		// than the free space and files larger than all the allocatable space.

		string->resize(max<UOffset>(size(), SIZE_MAX - 1)); // TODO: Make some String::MaxSize.
		if (read(string->text(), string->size()))
		{
			return true;
		}
	}

	return result;
}

File &File::operator =(const File &file)
{
	close();

	_private = Private::copy(file._private);
	_offset  = file._offset;
	_size    = file._size;
	_base    = file._base;

	return *this;
}

} // namespace Yttrium
