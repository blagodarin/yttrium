#include <yttrium/storage/temporary_file.h>

#include <yttrium/static_string.h>

#include <array>
#include <cstdlib>

#include "windows.h"

namespace Yttrium
{
	class TemporaryFilePrivate
	{
	public:
		TemporaryFilePrivate() // TODO: Throw on errors.
		{
			constexpr auto max_temp_path_size = MAX_PATH - 14; // GetTempFileName path length limit.
			std::array<char, max_temp_path_size + 1> path;
			if (!::GetTempPathA(path.size(), path.data()))
				std::abort();
			const auto status = ::GetTempFileNameA(path.data(), "ytt", 0, _name.data());
			if (!status || status == ERROR_BUFFER_OVERFLOW)
				std::abort();
		}

		~TemporaryFilePrivate() // TODO: Report failure.
		{
			::DeleteFileA(_name.data());
		}

	public:
		std::array<char, MAX_PATH> _name;
	};

	TemporaryFile::TemporaryFile()
		: _private(std::make_unique<TemporaryFilePrivate>())
	{
	}

	TemporaryFile::~TemporaryFile() = default;

	StaticString TemporaryFile::name() const
	{
		return _private->_name.data();
	}
}
