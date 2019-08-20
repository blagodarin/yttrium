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

#include <yttrium/storage/paths.h>

#include <cstdlib>

namespace
{
	std::filesystem::path home_directory_path()
	{
		if (const char* home = std::getenv("HOME"))
			return home;
		return std::filesystem::current_path(); // TODO: Get directory from getpwuid(getuid()).
	}
}

namespace Yt
{
	std::filesystem::path screenshots_path()
	{
		return ::home_directory_path();
	}

	std::filesystem::path user_data_path(std::string_view application_name)
	{
		std::string buffer = ".";
		buffer.reserve(buffer.size() + application_name.size());
		buffer += application_name;
		const auto home_path = ::home_directory_path();
		const auto data_path = home_path / buffer;
		std::filesystem::create_directory(data_path, home_path);
		return data_path;
	}

	std::filesystem::path user_export_path()
	{
		return ::home_directory_path();
	}
}
