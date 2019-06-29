//
// Copyright 2019 Sergei Blagodarin
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

#include "temporary_file.h"

#include <system_error>

#include <unistd.h>

namespace Yttrium
{
	TemporaryFilePrivate::TemporaryFilePrivate()
		: _name{ "/tmp/yttrium-XXXXXX" }
		, _descriptor{ ::mkstemp(_name.data()) }
	{
		if (_descriptor == -1)
			throw std::system_error{ errno, std::generic_category() };
	}

	TemporaryFilePrivate::~TemporaryFilePrivate() noexcept
	{
		if (::close(_descriptor))
			::perror("ERROR! 'close' failed");
		if (::unlink(_name.c_str()))
			::perror("ERROR! 'unlink' failed");
	}

	TemporaryFile::TemporaryFile()
		: _private{ std::make_unique<TemporaryFilePrivate>() }
	{
	}

	TemporaryFile::~TemporaryFile() = default;

	const std::string& TemporaryFile::name() const
	{
		return _private->_name;
	}
}
