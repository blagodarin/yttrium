#include "temporary_file.h"

#include <system_error>

#include <unistd.h>

namespace Yttrium
{
	TemporaryFilePrivate::TemporaryFilePrivate()
		: _name{ "/tmp/yttrium-XXXXXX" }
		, _descriptor{ ::mkstemp(_name.data()) }
	{
		if (_descriptor == -1)
			throw std::system_error{ errno, std::generic_category() };
	}

	TemporaryFilePrivate::~TemporaryFilePrivate() noexcept
	{
		if (::close(_descriptor))
			::perror("ERROR! 'close' failed");
		if (::unlink(_name.c_str()))
			::perror("ERROR! 'unlink' failed");
	}

	TemporaryFile::TemporaryFile()
		: _private{ std::make_unique<TemporaryFilePrivate>() }
	{
	}

	TemporaryFile::~TemporaryFile() = default;

	const std::string& TemporaryFile::name() const
	{
		return _private->_name;
	}
}
