#include "file.hpp"

#include <Yttrium/file_system.hpp>

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

bool File::open(const StaticString &name, FileSystem *file_system, Allocator *allocator)
{
	if (!file_system)
	{
		return open(name, File::Read, allocator);
	}

	*this = file_system->open_file(name);

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
