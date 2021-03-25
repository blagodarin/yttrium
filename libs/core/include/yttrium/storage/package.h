// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace Yt
{
	class Source;

	/// Package file types.
	enum class PackageType
	{
		Auto, ///< Automatical detection.
		Ypq,  ///< YPQ package.
	};

	/// Package reader interface.
	class PackageReader
	{
	public:
		///
		static std::unique_ptr<PackageReader> create(std::unique_ptr<Source>&&, PackageType = PackageType::Auto);

		virtual ~PackageReader() = default;

		///
		virtual const std::vector<std::string_view>& names() const = 0;

		///
		virtual std::unique_ptr<Source> open(std::size_t index) const = 0;
	};

	/// Package writer interface.
	class PackageWriter
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
