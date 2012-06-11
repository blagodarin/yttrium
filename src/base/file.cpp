#include "file.h"

#include <Yttrium/package.h>
#include <Yttrium/string.h>

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

bool File::read_line(String *string)
{
	static const size_t buffer_step = 32;

	if (!_private || (_private->mode & (Read | Pipe)) != Read)
	{
		return false; // NOTE: This only works for seekable files for now.
	}

	UOffset file_offset = _offset;

	string->clear();

	for (size_t offset = 0, bytes_read; ; offset += bytes_read)
	{
		string->resize(offset + buffer_step);

		bytes_read = read(string->text_at(offset), buffer_step);

		string->resize(offset + bytes_read);

		if (!bytes_read)
		{
			seek(file_offset + string->size());
			return offset;
		}

		size_t r_offset = string->find_first('\r', offset);

		if (r_offset != String::End)
		{
			size_t string_size = string->size();

			if (r_offset == string_size - 1)
			{
				string->resize(string_size + 1);
				bytes_read += read(string->text_at(string_size), 1);
			}

			string->resize(r_offset);
			seek(file_offset + r_offset + 1 + (string->const_at(r_offset + 1) == '\n'));
			break;
		}
		else
		{
			size_t n_offset = string->find_first('\n', offset);

			if (n_offset != String::End)
			{
				string->resize(n_offset);
				seek(file_offset + n_offset + 1);
				break;
			}
		}
	}

	return true;
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
