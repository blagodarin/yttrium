#define _FILE_OFFSET_BITS 64

#include <yttrium/utils.h>
#include "file.h"

#include <cassert>
#include <cstdlib>
#include <limits>
#include <new>
#include <system_error>

#include <fcntl.h>
#include <unistd.h>

namespace Yttrium
{
	SystemFile::SystemFile(String&& name, unsigned mode, uint64_t size, int descriptor)
		: FilePrivate(std::move(name), mode, size)
		, _descriptor(descriptor)
	{
		assert(_descriptor != -1);
	}

	SystemFile::~SystemFile()
	{
		if (_auto_close)
		{
			::close(_descriptor);
			if (_auto_remove && ::unlink(name().text()))
				throw std::system_error(errno, std::generic_category());
		}
	}

	bool SystemFile::flush()
	{
		return !::fsync(_descriptor);
	}

	size_t SystemFile::read(void* buffer, size_t size)
	{
		const auto result = ::read(_descriptor, buffer, size);
		return result != -1 ? result : 0;
	}

	size_t SystemFile::read(void* buffer, size_t size, uint64_t offset)
	{
		const auto result = ::pread(_descriptor, buffer, size, offset);
		return result != -1 ? result : 0;
	}

	bool SystemFile::resize(uint64_t size)
	{
		return !::ftruncate(_descriptor, size);
	}

	size_t SystemFile::write(const void* buffer, size_t size)
	{
		const auto result = ::write(_descriptor, buffer, size);
		return result != -1 ? result : 0;
	}

	size_t SystemFile::write(const void* buffer, size_t size, uint64_t offset)
	{
		const auto result = ::pwrite(_descriptor, buffer, size, offset);
		return result != -1 ? result : 0;
	}

	UniquePtr<FilePrivate> FilePrivate::open(const StaticString& path, unsigned mode, Allocator& allocator)
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

		String name(path, &allocator); // Guaranteed to be zero terminated.
		const auto descriptor = ::open(name.text(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (descriptor == -1)
			return {};

		const auto size = mode & File::Pipe ? 0 : ::lseek(descriptor, 0, SEEK_END);
		assert(size != -1); // TODO: Throw.
		return make_unique<SystemFile>(allocator, std::move(name), mode, size, descriptor);
	}

	UniquePtr<FilePrivate> FilePrivate::open(File::Special special, Allocator& allocator)
	{
		UniquePtr<SystemFile> result;
		switch (special)
		{
		case File::Temporary:
			{
				String name("/tmp/yttrium-XXXXXX"_s, &allocator);
				const auto descriptor = ::mkstemp(name.text());
				if (descriptor == -1)
					break;
				result = make_unique<SystemFile>(allocator, std::move(name), File::ReadWrite, 0, descriptor);
			}
			result->_auto_remove = true;
			break;

		case File::StdErr:
			result = make_unique<SystemFile>(allocator, String(&allocator), File::Write | File::Pipe, 0, STDERR_FILENO);
			result->_auto_close = false;
			break;
		}
		return std::move(result);
	}
}
