#ifndef __RENDERER_RENDERER_H
#define __RENDERER_RENDERER_H

#include <yttrium/margins.h>
#include <yttrium/pointer.h>
#include <yttrium/rect.h>
#include <yttrium/renderer.h>
#include <yttrium/string.h>
#include <yttrium/texture_font.h>

#include "../base/private_base.h"

#include <vector>

namespace Yttrium
{
	class Image;
	class WindowBackend;

	class RendererImpl: public Renderer
	{
	public:

		static std::unique_ptr<RendererImpl> create(WindowBackend& window, Allocator* allocator);

		RendererImpl(WindowBackend& window, Allocator* allocator);

		void draw_rectangle(const RectF& rect) override;
		void draw_rectangle(const RectF& rect, const RectF& texture_rect) override;
		void draw_text(const Vector2f& position, const StaticString& text, unsigned alignment, TextCapture* capture) override;
		void set_color(const Vector4f& color) override;
		bool set_font(const TextureFont& font) override;
		void set_font_size(const Vector2f& size) override;
		void set_texture(const Pointer<Texture2D>& texture) override;
		bool set_texture_borders(const MarginsI& borders) override;
		void set_texture_rectangle(const RectF& rect) override;
		Vector2f text_size(const StaticString& text) const override;
		Size window_size() const override;

		virtual void clear() = 0;
		virtual void take_screenshot(Image& image) = 0;

		Allocator* allocator() const { return _allocator; }
		void flush_2d();
		void pop_projection();
		void push_projection(const Matrix4f& matrix);
		void set_debug_texture();
		void set_window_size(const Size& size);

	protected:

		virtual void flush_2d_impl() = 0;
		virtual bool initialize() = 0;
		virtual void set_debug_texture_impl() = 0;
		virtual void set_projection(const Matrix4f& matrix) = 0;
		virtual void update_window_size() = 0;

	public:

		void draw_rectangle(const RectF &position, const RectF &texture, const MarginsF &borders);

	public:

		Allocator* const _allocator;
		WindowBackend& _window;

		Size     _window_size;

		Vector4f _color;

		// 2D rendering.

		struct Vertex2D
		{
			Vector2f position;
			Vector4f color;
			Vector2f texture;
		};

		std::vector<Vertex2D> _vertices_2d;
		std::vector<int16_t>  _indices_2d;

		Pointer<Texture2D> _texture;
		RectF              _texture_rect;
		MarginsF           _texture_borders;

		TextureFont _font;
		Vector2f    _font_size;

		bool _debug_rendering = false;

		std::vector<Matrix4f> _projection;
	};
}

#endif // __RENDERER_RENDERER_H
