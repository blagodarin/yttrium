// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

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
		auto data_path = home_path / buffer;
		std::filesystem::create_directory(data_path, home_path);
		return data_path;
	}

	std::filesystem::path user_export_path()
	{
		return ::home_directory_path();
	}
}
