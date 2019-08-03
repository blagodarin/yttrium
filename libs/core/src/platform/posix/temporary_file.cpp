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

#include <yttrium/storage/temporary_file.h>

#include "error.h"
#include "temporary_file.h"

#include <unistd.h>

namespace
{
	std::unique_ptr<Yttrium::TemporaryFilePrivate> create_temporary_file()
	{
		auto path = (std::filesystem::temp_directory_path() / "yt-XXXXXX").string();
		const auto descriptor = ::mkstemp(path.data());
		if (descriptor == -1)
			throw std::system_error{ errno, std::generic_category() };
		return std::make_unique<Yttrium::TemporaryFilePrivate>(std::move(path), descriptor); // TODO: Fix descriptor leak on exception.
	}
}

namespace Yttrium
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

	TemporaryFile::~TemporaryFile() = default;

	const std::filesystem::path& TemporaryFile::path() const
	{
		return _private->_path;
	}
}
