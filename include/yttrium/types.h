/// \file
/// \brief Basic types.

#ifndef __Y_TYPES_H
#define __Y_TYPES_H

#include <yttrium/global.h>

#include <cstddef> // ptrdiff_t, size_t
#include <cstdint> // int*_t, uint*_t, int_fast*_t, uint_fast*_t

namespace Yttrium
{

class Allocator;

template <typename T>
class Margins;

class Rect;

template <typename T>
class Rect_;

template <typename T>
class Vector2;

template <typename T>
class Vector4;

/// Generic integer type.

typedef int_fast32_t Integer;

/// Generic floating point type.

typedef double Real;

/// The default allocator.

extern Y_API Allocator* const DefaultAllocator;

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
enum class ExecutionMode
{
	Do,   ///<
	Undo, ///<
};

typedef Margins<int>    MarginsI;  ///< \c int Margins.
typedef Margins<float>  MarginsF;  ///< \c float Margins.

typedef Rect_<float>    RectF;     ///< \c float Rect.

typedef Vector2<float>  Vector2f;  ///< \c float Vector2.
typedef Vector2<double> Vector2d;  ///< \c double Vector2.
typedef Vector2<int>    Dim2;      ///< \c int Vector2.

typedef Vector4<float>  Vector4f;  ///< \c float Vector4.

} // namespace Yttrium

#endif // __Y_TYPES_H
