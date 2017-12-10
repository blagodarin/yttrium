#ifndef _include_yttrium_renderer_context_h_
#define _include_yttrium_renderer_context_h_

#include <yttrium/math/color.h>

#include <string_view>
#include <vector>

namespace Yttrium
{
	class Line3;
	class MarginsF;
	class Matrix4;
	class Mesh;
	class Quad;
	class RectF;
	class SizeF;
	class TexturedRect;
	class Vector2;

	///
	class RenderContext
	{
	public:
		virtual ~RenderContext() noexcept = default;

		///
		virtual void add_debug_text(std::string_view) = 0;

		///
		virtual void draw_mesh(const Mesh&) = 0;

		///
		virtual void draw_quad(const Quad&, const Color4f& = {1, 1, 1}) = 0;

		///
		virtual void draw_rect(const RectF&, const Color4f& = {1, 1, 1}) = 0;

		///
		virtual void draw_rects(const std::vector<TexturedRect>&, const Color4f& = {1, 1, 1}) = 0;

		///
		virtual Matrix4 full_matrix() const = 0;

		///
		virtual Matrix4 model_matrix() const = 0;

		///
		virtual Line3 pixel_ray(const Vector2&) const = 0;

		///
		virtual void set_texture_rect(const RectF&, const MarginsF&) = 0;

		///
		virtual SizeF window_size() const = 0;
	};
}

#endif
