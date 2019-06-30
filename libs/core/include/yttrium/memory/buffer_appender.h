//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <yttrium/memory/buffer.h>

namespace Yttrium
{
	///
	template <typename T>
	class BufferAppender
	{
	public:
		explicit BufferAppender(Buffer& buffer) noexcept
			: _buffer(buffer) {}

		///
		size_t count() const noexcept { return _buffer.size() / sizeof(T); }

		///
		BufferAppender& operator<<(const T& value)
		{
			_buffer.resize(_buffer.size() + sizeof value);
			*reinterpret_cast<T*>(_buffer.end() - sizeof value) = value;
			return *this;
		}

	private:
		Buffer& _buffer;
	};
}
