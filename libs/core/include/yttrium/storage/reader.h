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

#include <memory>
#include <string>

namespace Yttrium
{
	class Buffer;
	class Source;

	///
	class Y_CORE_API Reader
	{
	public:
		///
		explicit Reader(const Source& source) noexcept
			: _source{ source } {}

		/// Returns the current offset.
		uint64_t offset() const noexcept { return _offset; }

		/// Reads data from the source.
		size_t read(void*, size_t);

		/// Reads data from the source.
		template <typename T>
		bool read(T& data) { return read(&data, sizeof data) == sizeof data; }

		///
		bool read_line(std::string&);

		/// Sets the current offset.
		bool seek(uint64_t offset) noexcept;

		/// Returns the size of the source data.
		uint64_t size() const noexcept;

		/// Advances the current offset by the specified number of bytes.
		bool skip(uint64_t bytes) noexcept;

	private:
		const Source& _source;
		uint64_t _offset = 0;
	};
}
