/// \file
/// \brief Miscellaneous facilities.

#ifndef _include_yttrium_utils_h_
#define _include_yttrium_utils_h_

#include <cstdint>
#include <type_traits>

namespace Yttrium
{
	/// Returns \c true if the value is a power of two.
	template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
	constexpr bool is_power_of_2(T x)
	{
		return !(x & (x - 1)) && x > 0;
	}

	/// Returns the maximum of two values.
	/// \note Including <algorithm> for the alternative leads to noticeable compilation slowdown.
	template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
	constexpr T max(T x, T y)
	{
		return x > y ? x : y;
	}

	/// Returns the minimum of two values.
	/// \note Including <algorithm> for the alternative leads to noticeable compilation slowdown.
	template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
	constexpr T min(T x, T y)
	{
		return x < y ? x : y;
	}

	/// Returns the least power of two not less than the specified positive value.
	template <typename T, typename = std::enable_if_t<std::is_integral<T>::value && sizeof(T) <= sizeof(uint64_t)>>
	T next_power_of_2(T x)
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		if (sizeof x > sizeof(int8_t))
			x |= x >> 8;
		if (sizeof x > sizeof(int16_t))
			x |= x >> 16;
		if (sizeof x > sizeof(int32_t))
			x |= static_cast<uint64_t>(x) >> 32; // The conversion is to avoid pointless GCC warning.
		return x + 1;
	}

	/// Returns \c true if both values have the same sign.
	template <typename T, typename = std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value>>
	constexpr bool same_sign(T x, T y)
	{
		return (x ^ y) >= 0;
	}
}

#endif
