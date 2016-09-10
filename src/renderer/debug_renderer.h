#ifndef _src_renderer_debug_renderer_h_
#define _src_renderer_debug_renderer_h_

#include <yttrium/renderer/modifiers.h>

namespace Yttrium
{
	class RendererImpl;
	class StaticString;

	class DebugRenderer
	{
	public:
		DebugRenderer(RendererImpl& renderer);

		void draw_cursor(int x, int y);
		void draw_rectangle(int x, int y, int width, int height);
		void draw_text(int x, int y, const StaticString& text, int max_size = -1);
		int max_width() const;
		void set_color(float r, float g, float b, float a = 1);

	private:
		RendererImpl& _renderer;
		PushTexture _debug_texture;
	};
}

#endif
