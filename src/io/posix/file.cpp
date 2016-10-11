#define _FILE_OFFSET_BITS 64

#include "file.h"

#include <yttrium/static_string.h>

#include <cassert>
#include <cstdlib>
#include <limits>
#include <new>
#include <system_error>

#include <fcntl.h>
#include <unistd.h>

namespace Yttrium
{
	FilePrivate::FilePrivate(std::string&& name, unsigned mode, uint64_t size, int descriptor)
		: _name(std::move(name))
		, _mode(mode)
		, _size(size)
		, _descriptor(descriptor)
	{
		assert(_descriptor != -1);
	}

	FilePrivate::~FilePrivate()
	{
		if (_auto_close)
		{
			::close(_descriptor);
			if (_auto_remove && ::unlink(_name.c_str()))
				throw std::system_error(errno, std::generic_category());
		}
	}

	bool FilePrivate::flush()
	{
		return !::fsync(_descriptor);
	}

	size_t FilePrivate::read(void* buffer, size_t size)
	{
		const auto result = ::read(_descriptor, buffer, size);
		return result != -1 ? result : 0;
	}

	size_t FilePrivate::read_at(uint64_t offset, void* buffer, size_t size) const
	{
		const auto result = ::pread(_descriptor, buffer, size, offset);
		return result != -1 ? result : 0;
	}

	bool FilePrivate::resize(uint64_t size)
	{
		return !::ftruncate(_descriptor, size);
	}

	size_t FilePrivate::write(const void* buffer, size_t size)
	{
		const auto result = ::write(_descriptor, buffer, size);
		return result != -1 ? result : 0;
	}

	size_t FilePrivate::write_at(uint64_t offset, const void* buffer, size_t size)
	{
		const auto result = ::pwrite(_descriptor, buffer, size, offset);
		return result != -1 ? result : 0;
	}

	std::shared_ptr<FilePrivate> FilePrivate::open(const StaticString& path, unsigned mode)
	{
		int flags = Y_PLATFORM_LINUX ? O_NOATIME : 0;
		switch (mode & File::ReadWrite)
		{
		case File::Read: flags |= O_RDONLY;  break;
		case File::Write: flags = O_WRONLY | O_CREAT; break;
		case File::ReadWrite: flags |= O_RDWR | O_CREAT;  break;
		default: return {};
		}

		if ((mode & (File::Write | File::Pipe | File::Truncate)) == (File::Write | File::Truncate))
			flags |= O_TRUNC;

		std::string name(path.text(), path.size()); // Guaranteed to be zero terminated.
		const auto descriptor = ::open(name.c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (descriptor == -1)
			return {};

		const auto size = mode & File::Pipe ? 0 : ::lseek(descriptor, 0, SEEK_END);
		if (size == -1)
			throw std::system_error(errno, std::generic_category());

		return std::make_shared<FilePrivate>(std::move(name), mode, size, descriptor);
	}

	std::shared_ptr<FilePrivate> FilePrivate::open(File::Special special)
	{
		std::shared_ptr<FilePrivate> result;
		switch (special)
		{
		case File::Temporary:
			{
				std::string name("/tmp/yttrium-XXXXXX");
				const auto descriptor = ::mkstemp(const_cast<char*>(name.data())); // TODO: Remove const_cast in C++17.
				if (descriptor == -1)
					break;
				result = std::make_shared<FilePrivate>(std::move(name), File::ReadWrite, 0, descriptor);
			}
			result->_auto_remove = true;
			break;

		case File::StdErr:
			result = std::make_shared<FilePrivate>(std::string(), File::Write | File::Pipe, 0, STDERR_FILENO);
			result->_auto_close = false;
			break;
		}
		return std::move(result);
	}
}
