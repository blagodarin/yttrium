#include "file.hpp"

#include <Yttrium/package.hpp>

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

bool File::open(const StaticString &name, PackageManager *package_manager, Allocator *allocator)
{
	if (!package_manager)
	{
		return open(name, File::Read, allocator);
	}

	// TODO: Rewrite.

	*this = package_manager->open_file(name);

	return is_opened();
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
