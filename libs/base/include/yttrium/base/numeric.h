// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <type_traits>

namespace Yt
{
	/// Clamps a signed integer value to 8-bit unsigned one.
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>>>
	constexpr std::uint8_t clamp_to_uint8(T x) noexcept
	{
		return static_cast<std::uint8_t>(static_cast<std::make_unsigned_t<T>>(x) > 255 ? ~x >> (sizeof x * 8 - 1) : x);
	}

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
}
