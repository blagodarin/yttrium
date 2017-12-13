#ifndef _src_renderer_debug_renderer_h_
#define _src_renderer_debug_renderer_h_

#include <yttrium/math/color.h>
#include <yttrium/renderer/modifiers.h>

#include <optional>
#include <string_view>

namespace Yttrium
{
	class RenderPassImpl;

	class DebugRenderer
	{
	public:
		explicit DebugRenderer(RenderPass&);

		void draw_cursor(size_t x, size_t y);
		void draw_rectangle(size_t x, size_t y, size_t width, size_t height);
		void draw_text(size_t x, size_t y, std::string_view text, const std::optional<size_t>& max_size = {});
		size_t max_width() const;
		void set_color(float r, float g, float b, float a = 1);

	private:
		RenderPassImpl& _pass;
		Color4f _color{1, 1, 1};
		PushTexture _debug_texture;
	};
}

#endif
