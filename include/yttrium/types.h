/// \file
/// \brief Basic types.

#ifndef __Y_TYPES_H
#define __Y_TYPES_H

#include <yttrium/global.h>

namespace Yttrium
{

class Allocator;

/// The default allocator.
extern Y_API Allocator* const DefaultAllocator;

///
class ByReference {};

template <typename T>
class Margins;

typedef Margins<int> MarginsI; ///< \c int Margins.
typedef Margins<float> MarginsF; ///< \c float Margins.

class Rect;

template <typename T>
class Rect_;

typedef Rect_<float> RectF; ///< \c float Rect.

template <typename T>
class Vector2;

typedef Vector2<float>  Vector2f;  ///< \c float Vector2.
typedef Vector2<double> Vector2d;  ///< \c double Vector2.
typedef Vector2<int>    Dim2;      ///< \c int Vector2.

template <typename T>
class Vector4;

typedef Vector4<float>  Vector4f;  ///< \c float Vector4.

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

} // namespace Yttrium

#endif // __Y_TYPES_H
