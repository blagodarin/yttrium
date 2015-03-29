#ifndef __RENDERER_DEBUG_RENDERER_H
#define __RENDERER_DEBUG_RENDERER_H

#include <yttrium/vector.h>
#include <yttrium/renderer.h> // TODO: Remove when RendererImpl.

namespace Yttrium
{
	class DebugRenderer
	{
		friend Renderer;

	public:

		DebugRenderer(Renderer &renderer);
		~DebugRenderer();

		void draw_cursor(int x, int y);
		void draw_rectangle(int x, int y, int width, int height);
		void draw_text(int x, int y, const StaticString &text, int max_size = -1);
		void set_color(float r, float g, float b, float a = 1) { _color = Vector4f(r, g, b, a); }
		Dim2 size() const; // TODO: Rename.
		Dim2 text_size(const StaticString &text) const;

	private:

		Renderer::Private& _renderer;
		Vector4f _color;
	};
}

#endif // __RENDERER_DEBUG_RENDERER_H
