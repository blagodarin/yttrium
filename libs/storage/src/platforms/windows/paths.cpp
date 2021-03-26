// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/paths.h>

#include "../../../../core/src/platform/windows/error.h"

#include <primal/pointer.hpp>

#include <shlobj.h>
#include <versionhelpers.h>

namespace
{
	std::filesystem::path known_folder_path(const KNOWNFOLDERID& id)
	{
		primal::CPtr<wchar_t, ::CoTaskMemFree> path;
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
