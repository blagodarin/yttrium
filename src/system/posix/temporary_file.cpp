#include <yttrium/storage/temporary_file.h>

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
		std::string _name{"/tmp/yttrium-XXXXXX"};
		const int _descriptor = ::mkstemp(const_cast<char*>(_name.data())); // TODO-17: Remove const_cast.
	};

	TemporaryFile::TemporaryFile()
		: _private{std::make_unique<TemporaryFilePrivate>()}
	{
	}

	TemporaryFile::~TemporaryFile() = default;

	const std::string& TemporaryFile::name() const
	{
		return _private->_name;
	}
}
