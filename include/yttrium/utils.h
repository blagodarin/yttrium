#ifndef _include_yttrium_utils_h_
#define _include_yttrium_utils_h_

#include <cstdint>
#include <type_traits>

namespace Yttrium
{
	///
	template <typename I, typename P>
	constexpr I forward_find_if(I iterator, const P& predicate)
	{
		for (; !predicate(*iterator); ++iterator);
		return iterator;
	}

	/// Returns \c true if the value is a power of two.
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
	constexpr bool is_power_of_2(T x)
	{
		return !(x & (x - 1)) && x > 0;
	}

	/// Returns the least power of two not less than the specified positive value.
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T> && sizeof(T) <= sizeof(uint64_t)>>
	constexpr T next_power_of_2(T x)
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

	/// Returns \c true if both values have the same sign.
	template <typename T, typename = std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>>>
	constexpr bool same_sign(T x, T y)
	{
		return (x ^ y) >= 0;
	}

	///
	template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	constexpr auto to_underlying(T value)
	{
		return static_cast<std::underlying_type_t<T>>(value);
	}

	///
	template <typename T, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>>>
	constexpr T wrap(T value, U min, U max)
	{
		if (value < min)
			do { value += max - min; } while (value < min);
		else if (value >= max)
			do { value -= max - min; } while (value >= max);
		return value;
	}
}

#endif
