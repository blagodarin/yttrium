#include "file.h"

#include <yttrium/buffer.h>
#include <yttrium/package.h>
#include <yttrium/string.h>
#include <yttrium/utils.h>
#include "../package/manager.h"

namespace Yttrium
{
	File::~File()
	{
		Private::release(&_private);
	}

	File& File::operator=(File&& file)
	{
		Private::move(_private, file._private);
		_offset = file._offset;
		_size = file._size;
		_base = file._base;
		return *this;
	}

	File::operator bool() const
	{
		return _private && (_private->_mode & ReadWrite);
	}

	File::File(const StaticString& name, Allocator* allocator)
		: File(open_file_for_reading(name, *allocator))
	{
	}

	StaticString File::name() const
	{
		return _private->_name;
	}

	bool File::read_all(Buffer* buffer)
	{
		if (_private && (_private->_mode & Read))
		{
			// We should unconditionally crash if the file is too large.
			// This way we shall have the same behavior for both files larger
			// than the free space and files larger than all the allocatable space.
			buffer->resize(min<uint64_t>(size(), SIZE_MAX));
			if (read(buffer->data(), buffer->size()))
				return true;
		}
		return false;
	}

	bool File::read_all(String* string)
	{
		if (_private && (_private->_mode & Read))
		{
			// We should unconditionally crash if the file is too large.
			// This way we shall have the same behavior for both files larger
			// than the free space and files larger than all the allocatable space.

			string->resize(min<uint64_t>(size(), SIZE_MAX - 1)); // TODO: Make some String::MaxSize.
			if (read(string->text(), string->size()))
				return true;
		}
		return false;
	}

	bool File::read_line(String& string)
	{
		if (!_private || !(_private->_mode & Read))
			return false;

		if (_private->_mode & Pipe)
			return false; // TODO: Make it work for pipes too.

		static const size_t buffer_step = 32;

		uint64_t file_offset = _offset;

		string.clear();

		for (size_t offset = 0, bytes_read; ; offset += bytes_read)
		{
			string.resize(offset + buffer_step);

			bytes_read = read(string.text() + offset, buffer_step);

			string.resize(offset + bytes_read);

			if (!bytes_read)
			{
				seek(file_offset + string.size());
				return offset; // Return 'false' if we haven't read anything.
			}

			size_t r_offset = string.find_first('\r', offset);

			if (r_offset != String::End)
			{
				size_t string_size = string.size();

				if (r_offset == string_size - 1)
				{
					string.resize(string_size + 1);
					bytes_read += read(string.text() + string_size, 1);
				}

				string.resize(r_offset);
				seek(file_offset + r_offset + 1 + (string[r_offset + 1] == '\n'));
				break;
			}
			else
			{
				size_t n_offset = string.find_first('\n', offset);

				if (n_offset != String::End)
				{
					string.resize(n_offset);
					seek(file_offset + n_offset + 1);
					break;
				}
			}
		}

		return true;
	}

	File::File(Private* private_, uint64_t base, uint64_t size)
		: _private(Private::copy(private_))
		, _offset(0)
		, _size(size)
		, _base(base)
	{
	}
}
