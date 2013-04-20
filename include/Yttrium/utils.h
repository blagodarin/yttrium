/// \file
/// \brief Miscellaneous facilities.

#ifndef __Y_UTILS_H
#define __Y_UTILS_H

#include <Yttrium/assert.h>
#include <Yttrium/types.h>

namespace Yttrium
{

///

template <typename T>
const T &max(const T &a, const T &b) noexcept
{
	return a > b ? a : b;
}

///

template <typename T>
const T &min(const T &a, const T &b) noexcept
{
	return a < b ? a : b;
}

/// Clamp a value within the specified range.
/// \param a Source value.
/// \param lo Minimum value.
/// \param hi Maximum value.
/// \return Clamped value.

template <typename T>
const T &clamp(const T &a, const T &lo, const T &hi) noexcept
{
	Y_ASSERT(lo <= hi);
	return a > hi ? hi : (a < lo ? lo : a);
}

/// Determine if the specified value is a power of two (POT).
/// \param x Value to test.
/// \return Power-of-two test result.

inline bool is_power_of_2(int8_t x) noexcept
{
	return !(x & (x - 1)) && x > 0;
}

/**
* \overload
* \param x Value to test.
* \return Power-of-two test result.
*/

inline bool is_power_of_2(uint8_t x) noexcept
{
	return !(x & (x - 1)) && x > 0;
}

/**
* \overload
* \param x Value to test.
* \return Power-of-two test result.
*/

inline bool is_power_of_2(int16_t x) noexcept
{
	return !(x & (x - 1)) && x > 0;
}

/**
* \overload
* \param x Value to test.
* \return Power-of-two test result.
*/

inline bool is_power_of_2(uint16_t x) noexcept
{
	return !(x & (x - 1)) && x > 0;
}

/**
* \overload
* \param x Value to test.
* \return Power-of-two test result.
*/

inline bool is_power_of_2(int32_t x) noexcept
{
	return !(x & (x - 1)) && x > 0;
}

/**
* \overload
* \param x Value to test.
* \return Power-of-two test result.
*/

inline bool is_power_of_2(uint32_t x) noexcept
{
	return !(x & (x - 1)) && x > 0;
}

/**
* \overload
* \param x Value to test.
* \return Power-of-two test result.
*/

inline bool is_power_of_2(int64_t x) noexcept
{
	return !(x & (x - 1)) && x > 0;
}

/**
* \overload
* \param x Value to test.
* \return Power-of-two test result.
*/

inline bool is_power_of_2(uint64_t x) noexcept
{
	return !(x & (x - 1)) && x > 0;
}

/// Calculate the next power of two (POT) for the specified positive value.
/// \param x Source value.
/// \return Next power of two.

inline int8_t next_power_of_2(int8_t x) noexcept
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	return x + 1;
}

/**
* \overload
* \param x Source value.
* \return Next power of two.
*/

inline uint8_t next_power_of_2(uint8_t x) noexcept
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	return x + 1;
}

/**
* \overload
* \param x Source value.
* \return Next power of two.
*/

inline int16_t next_power_of_2(int16_t x) noexcept
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	return x + 1;
}

/**
* \overload
* \param x Source value.
* \return Next power of two.
*/

inline uint16_t next_power_of_2(uint16_t x) noexcept
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	return x + 1;
}

/**
* \overload
* \param x Source value.
* \return Next power of two.
*/

inline int32_t next_power_of_2(int32_t x) noexcept
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

/**
* \overload
* \param x Source value.
* \return Next power of two.
*/

inline uint32_t next_power_of_2(uint32_t x) noexcept
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

/**
* \overload
* \param x Source value.
* \return Next power of two.
*/

inline int64_t next_power_of_2(int64_t x) noexcept
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x |= x >> 32;
	return x + 1;
}

/**
* \overload
* \param x Source value.
* \return Next power of two.
*/

inline uint64_t next_power_of_2(uint64_t x) noexcept
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x |= x >> 32;
	return x + 1;
}

/// Determine if both values have the same sign.
/// \param x First value.
/// \param y Second value.
/// \return \c true if both values have the same sign.

// NOTE: Does it work for all the integer representations? If it does, add a doxy-note about it.

inline bool same_sign(int8_t x, int8_t y) noexcept
{
	return (x ^ y) >= 0;
}

/**
* \overload
* \param x First value.
* \param y Second value.
* \return \c true if both values have the same sign.
*/

inline bool same_sign(int16_t x, int16_t y) noexcept
{
	return (x ^ y) >= 0;
}

/**
* \overload
* \param x First value.
* \param y Second value.
* \return \c true if both values have the same sign.
*/

inline bool same_sign(int32_t x, int32_t y) noexcept
{
	return (x ^ y) >= 0;
}

/**
* \overload
* \param x First value.
* \param y Second value.
* \return \c true if both values have the same sign.
*/

inline bool same_sign(int64_t x, int64_t y) noexcept
{
	return (x ^ y) >= 0;
}

#if Y_HAS_CONSTEXPR || defined(__YTTRIUM_DOXYGEN)

/// Returns the number of elements in the array.

template <typename T, size_t N>
constexpr size_t countof(T (&)[N])
{
	return N;
}

#elif !defined(countof)

template <typename T, size_t N>
char (&_countof_helper(T (&)[N]))[N];

#define countof(array) (sizeof(_countof_helper(array)))

#endif

} // namespace Yttrium

#endif // __Y_UTILS_H
