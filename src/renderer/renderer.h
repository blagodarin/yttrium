#ifndef __RENDERER_RENDERER_H
#define __RENDERER_RENDERER_H

#include <yttrium/renderer.h>

#include <yttrium/margins.h>
#include <yttrium/pointer.h>
#include <yttrium/string.h>
#include <yttrium/texture_font.h>
#include "../base/private_base.h"

#include <vector>

namespace Yttrium
{
	class BackendTexture2D;
	class Image;
	class WindowBackend;

	class RendererImpl: public Renderer
	{
	public:

		static std::unique_ptr<RendererImpl> create(WindowBackend& window, Allocator* allocator);

		RendererImpl(WindowBackend& window, Allocator* allocator);

		void draw_rectangle(const RectF& rect) override;
		void draw_rectangle(const RectF& rect, const RectF& texture_rect) override;
		void draw_text(const Vector2& position, const StaticString& text, unsigned alignment, TextCapture* capture) override;
		void set_color(const Vector4& color) override;
		bool set_font(const TextureFont& font) override;
		void set_font_size(const Vector2& size) override;
		bool set_texture_borders(const Margins& borders) override;
		void set_texture_rectangle(const RectF& rect) override;
		Vector2 text_size(const StaticString& text) const override;
		Size window_size() const override;

		virtual void clear() = 0;
		virtual void take_screenshot(Image& image) = 0;

		Allocator* allocator() const { return _allocator; }
		void flush_2d();
		void pop_projection();
		void pop_texture();
		void pop_transformation();
		void push_projection(const Matrix4& matrix);
		void push_texture(const Pointer<Texture2D>& texture);
		void push_transformation(const Matrix4& matrix);
		void set_debug_texture();
		void set_window_size(const Size& size);

	protected:

		virtual void flush_2d_impl() = 0;
		virtual bool initialize() = 0;
		virtual void set_debug_texture_impl() = 0;
		virtual void set_projection(const Matrix4& matrix) = 0;
		virtual void set_transformation(const Matrix4& matrix) = 0;
		virtual void update_window_size() = 0;

	public:

		void change_texture(const Pointer<Texture2D>& old_texture, const Pointer<Texture2D>& new_texture);
		BackendTexture2D* current_texture_2d() const;
		void draw_rectangle(const RectF& position, const RectF& texture, const MarginsF& borders);

	public:

		Allocator* const _allocator;
		WindowBackend& _window;

		Size _window_size;

		Vector4 _color;

		// 2D rendering.

		struct Vertex2D
		{
			Vector2 position;
			Vector4 color;
			Vector2 texture;
		};

		std::vector<Vertex2D> _vertices_2d;
		std::vector<int16_t>  _indices_2d;

		RectF    _texture_rect;
		MarginsF _texture_borders;

		TextureFont _font;
		Vector2     _font_size;

		bool _debug_rendering = false;

		std::vector<Matrix4> _projection;
		std::vector<std::pair<Pointer<Texture2D>, int>> _texture_stack;
		std::vector<Matrix4> _transformation;
	};
}

#endif // __RENDERER_RENDERER_H
