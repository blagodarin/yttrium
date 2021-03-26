// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <string>

namespace Yt
{
	class RingLog
	{
	public:
		static constexpr size_t BufferSize = 4096;
		static constexpr size_t MaxStringSize = std::numeric_limits<uint8_t>::max();

		[[nodiscard]] constexpr bool empty() const noexcept { return !_size; }
		bool pop(std::string&);
		void push(std::string_view) noexcept;

	private:
		size_t _offset = 0;
		size_t _size = 0;
		std::array<uint8_t, BufferSize> _buffer;
	};
}
