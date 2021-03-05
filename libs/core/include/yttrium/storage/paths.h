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

#pragma once

#include <yttrium/api.h>

#include <filesystem>

namespace Yt
{
	//! Returns the default path for screenshots.
	Y_CORE_API std::filesystem::path screenshots_path();

	//! Path for user-specific data (e. g. configuration files or saved state).
	Y_CORE_API std::filesystem::path user_data_path(std::string_view application_name);

	//! Path for exported content (e. g. screenshots).
	Y_CORE_API std::filesystem::path user_export_path();
}