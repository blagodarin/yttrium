// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <filesystem>
#include <memory>

namespace Yt
{
	class TemporaryFilePrivate
	{
	public:
		TemporaryFilePrivate(std::string&& path, int descriptor)
			: _path{ std::move(path) }, _descriptor{ descriptor } {}
		~TemporaryFilePrivate() noexcept;

		static auto descriptor(const TemporaryFile& file) noexcept { return file._private->_descriptor; }

	public:
		const std::filesystem::path _path;
		const int _descriptor;
	};
}
