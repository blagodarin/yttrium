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

#include <yttrium/memory/smart_ptr.h>
#include "error.h"

#include <shlobj.h>
#include <versionhelpers.h>

namespace
{
	std::filesystem::path known_folder_path(const KNOWNFOLDERID& id)
	{
		Yt::SmartPtr<wchar_t, ::CoTaskMemFree> path;
		if (const auto hr = ::SHGetKnownFolderPath(id, KF_FLAG_CREATE, nullptr, path.out()); FAILED(hr))
		{
			Yt::log_error("SHGetKnownFolderPath", hr);
			return std::filesystem::current_path();
		}
		return path.get();
	}
}

namespace Yt
{
	std::filesystem::path screenshots_path()
	{
		return ::known_folder_path(::IsWindows8OrGreater() ? FOLDERID_Screenshots : FOLDERID_Pictures);
	}

	std::filesystem::path user_data_path(std::string_view application_name)
	{
		const auto path = ::known_folder_path(FOLDERID_LocalAppData) / std::filesystem::u8path(application_name);
		std::filesystem::create_directory(path);
		return path;
	}

	std::filesystem::path user_export_path()
	{
		return ::known_folder_path(FOLDERID_Documents);
	}
}
