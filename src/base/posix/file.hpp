#ifndef __BASE_POSIX_FILE_HPP
#define __BASE_POSIX_FILE_HPP

#include <atomic>

#include <Yttrium/file.hpp>

namespace Yttrium
{

class File::Private
{
public:

	Allocator           *allocator;
	std::atomic<size_t>  references;
	int                  descriptor;
	Mode                 mode;

public:

	Private(int descriptor = -1, Mode mode = 0, Allocator *allocator = nullptr) noexcept
		: allocator(allocator)
		, references(1)
		, descriptor(descriptor)
		, mode(mode)
	{
	}

public:

	static int open(const StaticString &name, int flags, Allocator *allocator) noexcept;
};

class StaticFile: public File
{
public:

	StaticFile() noexcept
		: File(&_private_data)
	{
	}

	StaticFile(const StaticString &name, Mode mode) noexcept
		: File(&_private_data)
	{
		File::open(name, mode, nullptr);
	}

	StaticFile(const StaticFile &file) = delete;

	StaticFile &operator =(const StaticFile &file) = delete;

	bool open(const StaticString &name, Mode mode) noexcept
	{
		return File::open(name, mode, nullptr);
	}

private:

	Private _private_data;
};

} // namespace Yttrium

#endif // __BASE_POSIX_FILES_HPP
