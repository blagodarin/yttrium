// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/storage/package.h>

namespace Yt
{
	class Buffer;
	class Source;

	///
	class Storage
	{
	public:
		///
		enum class UseFileSystem
		{
			After,
			Before,
			Never,
		};

		///
		explicit Storage(UseFileSystem);

		///
		~Storage();

		/// Attaches a buffer to the storage.
		void attach_buffer(std::string_view, Buffer&&);

		/// Attaches a package to the storage.
		/// Throws MissingDataError if there is no such package.
		void attach_package(const std::filesystem::path&, PackageType = PackageType::Auto);

		/// Opens a data source.
		std::unique_ptr<Source> open(std::string_view name) const;

	private:
		const std::unique_ptr<class StoragePrivate> _private;
	};
}
