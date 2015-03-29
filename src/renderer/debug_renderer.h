#ifndef __RENDERER_DEBUG_RENDERER_H
#define __RENDERER_DEBUG_RENDERER_H

#include <yttrium/types.h>
#include <yttrium/vector.h>

namespace Yttrium
{
	class RendererImpl;
	class StaticString;

	class DebugRenderer
	{
	public:

		DebugRenderer(RendererImpl& renderer);
		~DebugRenderer();

		void draw_cursor(int x, int y);
		void draw_rectangle(int x, int y, int width, int height);
		void draw_text(int x, int y, const StaticString& text, int max_size = -1);
		void set_color(float r, float g, float b, float a = 1) { _color = Vector4f(r, g, b, a); }
		Dim2 size() const; // TODO: Rename.
		Dim2 text_size(const StaticString &text) const;

	private:

		RendererImpl& _renderer;
		Vector4f _color;
	};
}

#endif // __RENDERER_DEBUG_RENDERER_H
