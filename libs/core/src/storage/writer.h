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

#include <cstdint>
#include <cstddef>

namespace Yt
{
	class Buffer;
	class Writer;

	class WriterPrivate
	{
	public:
		WriterPrivate() noexcept = default;
		virtual ~WriterPrivate() noexcept = default;

		virtual bool try_reserve(uint64_t) noexcept { return true; }
		virtual size_t write_at(uint64_t, const void*, size_t) noexcept = 0;

	private:
		uint64_t _size = 0;
		uint64_t _offset = 0;

		friend Writer;
	};

	class BufferWriter final : public WriterPrivate
	{
	public:
		explicit BufferWriter(Buffer&) noexcept;

		bool try_reserve(uint64_t) noexcept override;
		size_t write_at(uint64_t, const void*, size_t) noexcept override;

	private:
		Buffer& _buffer;
	};
}
