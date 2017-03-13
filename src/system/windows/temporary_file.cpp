#include <yttrium/storage/temporary_file.h>

#include <array>
#include <cstdlib>

#include "windows.h"

namespace
{
	std::string make_temporary_file_name() // TODO: Throw on errors.
	{
		constexpr auto max_temp_path_size = MAX_PATH - 14; // GetTempFileName path length limit.
		std::array<char, max_temp_path_size + 1> path;
		static_assert(path.size() <= std::numeric_limits<DWORD>::max(), "Invalid MAX_PATH value"); // TODO-17: Remove the message.
		if (!::GetTempPathA(static_cast<DWORD>(path.size()), path.data()))
			std::abort();
		std::array<char, MAX_PATH> name;
		const auto status = ::GetTempFileNameA(path.data(), "ytt", 0, name.data());
		if (!status || status == ERROR_BUFFER_OVERFLOW)
			std::abort();
		return name.data();
	}
}

namespace Yttrium
{
	class TemporaryFilePrivate
	{
	public:
		~TemporaryFilePrivate()
		{
			::DeleteFileA(_name.data()); // TODO: Report failure.
		}

	public:
		const std::string _name = ::make_temporary_file_name();
	};

	TemporaryFile::TemporaryFile()
		: _private(std::make_unique<TemporaryFilePrivate>())
	{
	}

	TemporaryFile::~TemporaryFile() = default;

	const std::string& TemporaryFile::name() const
	{
		return _private->_name;
	}
}
