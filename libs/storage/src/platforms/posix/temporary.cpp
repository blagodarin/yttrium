// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/temporary.h>

#include "../../../../base/src/posix/error.h"
#include "temporary.h"

#include <unistd.h>

namespace
{
	std::unique_ptr<Yt::TemporaryFilePrivate> create_temporary_file()
	{
		auto path = (std::filesystem::temp_directory_path() / "yt-XXXXXX").string();
		const auto descriptor = ::mkstemp(path.data());
		if (descriptor == -1)
			throw std::system_error{ errno, std::generic_category() };
		return std::make_unique<Yt::TemporaryFilePrivate>(std::move(path), descriptor); // TODO: Fix descriptor leak on exception.
	}
}

namespace Yt
{
	TemporaryFilePrivate::~TemporaryFilePrivate() noexcept
	{
		if (::close(_descriptor))
			report_errno("close");
		if (::unlink(_path.c_str()))
			report_errno("unlink");
	}

	TemporaryFile::TemporaryFile()
		: _private{ ::create_temporary_file() }
	{
	}

	TemporaryFile::~TemporaryFile() noexcept = default;

	const std::filesystem::path& TemporaryFile::path() const
	{
		return _private->_path;
	}
}
