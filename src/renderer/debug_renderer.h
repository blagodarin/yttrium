#ifndef _src_renderer_debug_renderer_h_
#define _src_renderer_debug_renderer_h_

#include <yttrium/math/color.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/std/optional.h>
#include <yttrium/std/string_view.h>

namespace Yttrium
{
	class RendererImpl;

	class DebugRenderer
	{
	public:
		explicit DebugRenderer(RendererImpl&);

		void draw_cursor(int x, int y);
		void draw_rectangle(int x, int y, int width, int height);
		void draw_text(int x, int y, std::string_view text, const std::optional<size_t>& max_size = {});
		size_t max_width() const;
		void set_color(float r, float g, float b, float a = 1);

	private:
		RendererImpl& _renderer;
		Color4f _color{1, 1, 1};
		PushTexture _debug_texture;
	};
}

#endif
