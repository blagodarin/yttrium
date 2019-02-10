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

#ifndef _include_yttrium_utils_numeric_h_
#define _include_yttrium_utils_numeric_h_

#include <cstdint>
#include <type_traits>

namespace Yttrium
{
	/// Returns \c true if the value is a power of two.
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
	constexpr bool is_power_of_2(T x) noexcept
	{
		return !(x & (x - 1)) && x > 0;
	}

	/// Returns the least power of two not less than the specified positive value.
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T> && sizeof(T) <= sizeof(uint64_t)>>
	constexpr T next_power_of_2(T x) noexcept
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		if constexpr (sizeof x > sizeof(int8_t))
			x |= x >> 8;
		if constexpr (sizeof x > sizeof(int16_t))
			x |= x >> 16;
		if constexpr (sizeof x > sizeof(int32_t))
			x |= static_cast<std::conditional_t<std::is_signed_v<T>, int64_t, uint64_t>>(x) >> 32;
		return x + 1;
	}

	///
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
	constexpr T power_of_2_alignment(T x) noexcept
	{
		return static_cast<T>(((x ^ (x - 1)) + 1) >> 1);
	}

	/// Returns \c true if both values have the same sign.
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>>>
	constexpr bool same_sign(T x, T y) noexcept
	{
		return (x ^ y) >= 0;
	}

	///
	constexpr std::uint16_t swap_bytes(std::uint16_t x) noexcept { return static_cast<std::uint16_t>(x >> 8 | x << 8); }
	constexpr std::uint32_t swap_bytes(std::uint32_t x) noexcept { return x << 24 | (x & 0xff00) << 8 | (x & 0xff0000) >> 8 | x >> 24; }

	///
	template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	constexpr auto to_underlying(T value) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(value);
	}

	///
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>>>
	constexpr std::make_unsigned_t<T> to_unsigned(T value) noexcept
	{
		return static_cast<std::make_unsigned_t<T>>(value);
	}

	///
	template <typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
	constexpr T wrap(T value, U min, U max) noexcept
	{
		if (value < min)
			do
			{
				value += max - min;
			} while (value < min);
		else if (value >= max)
			do
			{
				value -= max - min;
			} while (value >= max);
		return value;
	}
}

#endif
