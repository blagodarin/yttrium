/// \file
/// \brief Basic types.

#ifndef __Y_TYPES_H
#define __Y_TYPES_H

#include <yttrium/global.h>

#define __STDC_LIMIT_MACROS    // *_MIN, *_MAX
#define __STDC_CONSTANT_MACROS // INT*_C, UINT*_C

#include <cstddef>  // ptrdiff_t, size_t
#include <stdint.h> // int*_t, uint*_t, int_fast*_t, uint_fast*_t

namespace Yttrium
{

class Allocator;

template <typename T>
class Margins;

template <typename T>
class Rect;

template <typename T>
class Vector2;

template <typename T>
class Vector3;

template <typename T>
class Vector4;

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

/// The default allocator.

extern Y_API Allocator *const DefaultAllocator;

///

class ByReference
{
};

enum
{
	CenterAlignment = 0, ///< Center alignment.

	LeftAlignment   = 1 << 0, ///< Left alignment.
	RightAlignment  = 1 << 1, ///< Right alignment.
	TopAlignment    = 1 << 2, ///< Top alignment.
	BottomAlignment = 1 << 3, ///< Bottom alignment.

	TopLeftAlignment     = TopAlignment | LeftAlignment,     ///< Top left alignment.
	TopRightAlignment    = TopAlignment | RightAlignment,    ///< Top right alignment.
	BottomLeftAlignment  = BottomAlignment | LeftAlignment,  ///< Bottom left alignment.
	BottomRightAlignment = BottomAlignment | RightAlignment, ///< Bottom right alignment.

	HorizontalAlignmentMask = LeftAlignment | RightAlignment, ///< Horizontal alignment mask.
	VerticalAlignmentMask   = TopAlignment | BottomAlignment, ///< Vertical alignment mask.
};

///

typedef uint_fast8_t Alignment;

///

enum class ExecutionMode
{
	Do,   ///<
	Undo, ///<
};

// TODO: Find a better name for Area.

typedef Margins<Integer> MarginsI;  ///< Integer Margins.

typedef Rect<Integer>    RectI;     ///< Integer Rect.
typedef Rect<float>      RectF;     ///< \c float Rect.
typedef Rect<Dim>        Area;      ///< Dim Rect.

typedef Vector2<int8_t>  Vector2i8; ///< \c int8_t Vector2.
typedef Vector2<uint8_t> Vector2u8; ///< \c uint8_t Vector2.
typedef Vector2<float>   Vector2f;  ///< \c float Vector2.
typedef Vector2<double>  Vector2d;  ///< \c double Vector2.
typedef Vector2<Dim>     Dim2;      ///< Dim Vector2.

typedef Vector3<float>   Vector3f;  ///< \c float Vector3.

typedef Vector4<float>   Vector4f;  ///< \c float Vector4.

} // namespace Yttrium

#endif // __Y_TYPES_H
