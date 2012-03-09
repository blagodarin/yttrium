#include "file.hpp"

namespace Yttrium
{

File::File(const File &file)
	: _private(Private::copy(file._private))
	, _offset(file._offset)
	, _size(file._size)
	, _base(file._base)
{
}

bool File::opened() const
{
	return (_private && (_private->mode & ReadWrite));
}

} // namespace Yttrium
