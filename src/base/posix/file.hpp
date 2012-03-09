#ifndef __BASE_POSIX_FILE_HPP
#define __BASE_POSIX_FILE_HPP

#include <Yttrium/file.hpp>

#include "../private_base.hpp"

namespace Yttrium
{

class File::Private: public PrivateBase<File::Private>
{
public:

	int  descriptor;
	Mode mode;

public:

	Private(int descriptor = -1, Mode mode = 0, Allocator *allocator = nullptr)
		: PrivateBase(allocator)
		, descriptor(descriptor)
		, mode(mode)
	{
	}

public:

	static int open(const StaticString &name, int flags, Allocator *allocator);
};

class StaticFile: public File
{
public:

	StaticFile()
		: File(&_private_data)
	{
	}

	StaticFile(const StaticString &name, Mode mode)
		: File(&_private_data)
	{
		File::open(name, mode, nullptr);
	}

	StaticFile(const StaticFile &) = delete;

	StaticFile &operator =(const StaticFile &) = delete;

	bool open(const StaticString &name, Mode mode)
	{
		return File::open(name, mode, nullptr);
	}

private:

	Private _private_data;
};

} // namespace Yttrium

#endif // __BASE_POSIX_FILES_HPP
