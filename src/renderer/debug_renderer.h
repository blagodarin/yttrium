#ifndef _src_renderer_debug_renderer_h_
#define _src_renderer_debug_renderer_h_

#include <yttrium/math/color.h>
#include <yttrium/renderer/modifiers.h>

#include <boost/optional/optional.hpp>

namespace Yttrium
{
	class RendererImpl;
	class StaticString;

	class DebugRenderer
	{
	public:
		explicit DebugRenderer(RendererImpl& renderer);

		void draw_cursor(int x, int y);
		void draw_rectangle(int x, int y, int width, int height);
		void draw_text(int x, int y, const StaticString& text, const boost::optional<size_t>& max_size = {});
		size_t max_width() const;
		void set_color(float r, float g, float b, float a = 1);

	private:
		RendererImpl& _renderer;
		Color4f _color{1, 1, 1};
		PushTexture _debug_texture;
	};
}

#endif
