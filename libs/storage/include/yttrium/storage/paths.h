// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <filesystem>

namespace Yt
{
	//! Returns the default path for screenshots.
	std::filesystem::path screenshots_path();

	//! Path for user-specific data (e. g. configuration files or saved state).
	std::filesystem::path user_data_path(std::string_view application_name);

	//! Path for exported content (e. g. screenshots).
	std::filesystem::path user_export_path();
}
