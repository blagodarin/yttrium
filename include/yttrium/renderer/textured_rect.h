/// \file
/// \brief

#ifndef _include_yttrium_renderer_textured_rect_h_
#define _include_yttrium_renderer_textured_rect_h_

#include <yttrium/math/rect.h>

namespace Yttrium
{
	///
	class TexturedRect
	{
	public:
		RectF geometry; ///<
		RectF texture;  ///<

		TexturedRect() = default;
		TexturedRect(const RectF& geometry, const RectF& texture) : geometry(geometry), texture(texture) {}
	};
}

#endif
