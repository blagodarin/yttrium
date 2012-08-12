/// \file
/// \brief Basic types.

#ifndef __Y_TYPES_H
#define __Y_TYPES_H

#include <Yttrium/global.h>

#define __STDC_LIMIT_MACROS    // *_MIN, *_MAX
#define __STDC_CONSTANT_MACROS // INT*_C, UINT*_C

#include <cstddef>  // ptrdiff_t, size_t
#include <stdint.h> // int*_t, uint*_t, int_fast*_t, uint_fast*_t

namespace Yttrium
{

/// Unsigned integer type of \c char size.

typedef unsigned char UChar;

/// Signed integer type of \c char size.

typedef signed char SChar;

/// Signed file offset type.

typedef int_fast64_t Offset;

/// Unsigned file offset type.

typedef uint_fast64_t UOffset;

/// Integer type for representing graphical dimensions.

typedef int_fast16_t Dim;

/// Generic integer type.

typedef int_fast32_t Integer;

/// Generic floating point type.

typedef double Real;

} // namespace Yttrium

#endif // __Y_TYPES_H
