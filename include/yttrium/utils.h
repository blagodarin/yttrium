/// \file
/// \brief Miscellaneous facilities.

#ifndef __Y_UTILS_H
#define __Y_UTILS_H

#include <yttrium/global.h>

#include <cstdint>

// TODO: Check whether all functions in this file work with all supported integer representations,
// documenting the results.

namespace Yttrium
{

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

} // namespace Yttrium

#endif // __Y_UTILS_H
