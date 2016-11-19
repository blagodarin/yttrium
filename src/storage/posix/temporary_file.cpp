#include <yttrium/storage/temporary_file.h>

#include <yttrium/static_string.h>

#include <system_error>

#include <unistd.h>

namespace Yttrium
{
	class TemporaryFilePrivate
	{
	public:
		TemporaryFilePrivate()
		{
			if (_descriptor == -1)
				throw std::system_error(errno, std::generic_category());
		}

		~TemporaryFilePrivate()
		{
			if (::close(_descriptor))
				::perror("ERROR! 'close' failed");
			if (::unlink(_name.c_str()))
				::perror("ERROR! 'unlink' failed");
		}

	public:
		std::string _name{ "/tmp/yttrium-XXXXXX" };
		const int _descriptor = ::mkstemp(&_name[0]); // TODO: Use data() in C++17.
	};

	TemporaryFile::TemporaryFile()
		: _private(std::make_unique<TemporaryFilePrivate>())
	{
	}

	TemporaryFile::~TemporaryFile() = default;

	StaticString TemporaryFile::name() const
	{
		return StaticString(_private->_name.data(), _private->_name.size());
	}
}
