// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <filesystem>
#include <memory>

namespace Yt
{
	class TemporaryFile;
	class WriterPrivate;

	std::unique_ptr<WriterPrivate> create_file_writer(const std::filesystem::path&);
	std::unique_ptr<WriterPrivate> create_file_writer(TemporaryFile&);
}
