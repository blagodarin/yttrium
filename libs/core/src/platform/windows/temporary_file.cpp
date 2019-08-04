//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/storage/temporary_file.h>

#include "error.h"

#include <array>

#include <windows.h>

namespace
{
	std::filesystem::path make_temporary_file_name()
	{
		constexpr auto max_temp_path_size = MAX_PATH - 14; // GetTempFileName path length limit.
		std::array<wchar_t, max_temp_path_size + 1> path;
		static_assert(path.size() <= std::numeric_limits<DWORD>::max());
		if (!::GetTempPathW(static_cast<DWORD>(path.size()), path.data()))
			throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
		std::array<wchar_t, MAX_PATH> name;
		const auto status = ::GetTempFileNameW(path.data(), L"yt-", 0, name.data());
		if (!status)
			throw std::system_error{ static_cast<int>(::GetLastError()), std::system_category() };
		else if (status == ERROR_BUFFER_OVERFLOW)
			throw std::system_error{ static_cast<int>(status), std::system_category() };
		return name.data();
	}
}

namespace Yttrium
{
	class TemporaryFilePrivate
	{
	public:
		~TemporaryFilePrivate() noexcept
		{
			if (!::DeleteFileW(_path.c_str()))
				log_last_error("DeleteFile");
		}

	public:
		const std::filesystem::path _path = ::make_temporary_file_name();
	};

	TemporaryFile::TemporaryFile()
		: _private{ std::make_unique<TemporaryFilePrivate>() }
	{
	}

	TemporaryFile::~TemporaryFile() = default;

	const std::filesystem::path& TemporaryFile::path() const
	{
		return _private->_path;
	}
}
