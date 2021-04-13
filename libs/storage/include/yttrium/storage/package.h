// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string_view>
#include <vector>

namespace Yt
{
	class Source;

	/// Package reader interface.
	class PackageReader
	{
	public:
		///
		static std::unique_ptr<PackageReader> create(std::unique_ptr<Source>&&);

		virtual ~PackageReader() = default;

		///
		virtual const std::vector<std::string_view>& names() const = 0;

		///
		virtual std::unique_ptr<Source> open(size_t index) const = 0;
	};
}
