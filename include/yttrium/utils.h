/// \file
/// \brief Miscellaneous facilities.

#ifndef __Y_UTILS_H
#define __Y_UTILS_H

#include <yttrium/global.h>

#include <cstdint>
#include <type_traits>

namespace Yttrium
{
	/// Returns \c true if the value is a power of two.
	template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
	bool is_power_of_2(T x)
	{
		return !(x & (x - 1)) && x > 0;
	}

	/// Returns the least power of two not less than the specified positive value.
	template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
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
			x |= static_cast<uintmax_t>(x) >> 32; // The conversion is to avoid pointless GCC warning.
		return x + 1;
	}

	/// Returns \c true if both values have the same sign.
	template <typename T, typename = std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value>>
	bool same_sign(T x, T y)
	{
		return (x ^ y) >= 0;
	}
}

#endif // __Y_UTILS_H
