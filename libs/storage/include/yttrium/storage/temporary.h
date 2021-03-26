// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <filesystem>
#include <memory>

namespace Yt
{
	///
	class TemporaryFile
	{
	public:
		///
		TemporaryFile();

		///
		~TemporaryFile() noexcept;

		///
		const std::filesystem::path& path() const;

	private:
		const std::unique_ptr<class TemporaryFilePrivate> _private;
		friend TemporaryFilePrivate;
	};
}
