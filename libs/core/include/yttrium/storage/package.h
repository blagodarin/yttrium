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
#include <memory>
#include <string>
#include <vector>

namespace Yttrium
{
	class Source;

	/// Package file types.
	enum class PackageType
	{
		Auto, ///< Automatical detection.
		Ypq,  ///< YPQ package.
	};

	/// Package reader interface.
	class Y_CORE_API PackageReader
	{
	public:
		///
		static std::unique_ptr<PackageReader> create(const std::filesystem::path&, PackageType = PackageType::Auto);

		virtual ~PackageReader() = default;

		///
		virtual const std::vector<std::string_view>& names() const = 0;

		///
		virtual std::unique_ptr<Source> open(std::size_t index) const = 0;
	};

	/// Package writer interface.
	class Y_CORE_API PackageWriter
	{
	public:
		///
		static std::unique_ptr<PackageWriter> create(const std::filesystem::path&, PackageType = PackageType::Auto);

		virtual ~PackageWriter() = default;

		///
		virtual bool add(const std::string&) = 0;

		//! Writes added files into the package.
		//! If fails, leaves the package file with unspecified contents.
		virtual bool commit() = 0;
	};
}
