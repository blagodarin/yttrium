#ifndef __RENDERER_DEBUG_RENDERER_H
#define __RENDERER_DEBUG_RENDERER_H

namespace Yttrium
{
	class RendererImpl;
	class Size;
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
	};
}

#endif // __RENDERER_DEBUG_RENDERER_H
