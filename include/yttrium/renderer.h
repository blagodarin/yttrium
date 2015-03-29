/// \file
/// \brief

#ifndef __Y_RENDERER_H
#define __Y_RENDERER_H

#include <yttrium/rect.h>

#include <memory>

namespace Yttrium
{
	class StaticString;
	class Texture2D;
	class TextureCache;
	class TextureFont;

	template <typename T>
	class Pointer;

	///
	class Renderer
	{
	public:

		///
		struct TextCapture
		{
			const unsigned cursor_pos;
			bool           has_cursor;
			RectF          cursor_rect;

			const unsigned selection_begin;
			const unsigned selection_end;
			bool           has_selection;
			RectF          selection_rect;

			TextCapture(unsigned cursor_pos, unsigned selection_begin, unsigned selection_size)
				: cursor_pos(cursor_pos)
				, has_cursor(false)
				, selection_begin(selection_begin)
				, selection_end(selection_begin + selection_size)
				, has_selection(false)
			{
			}
		};

		Renderer() = default;
		virtual ~Renderer() = default;

		///
		/// \note Texture cache lifetime must not exceed its renderer lifetime.
		virtual std::unique_ptr<TextureCache> create_texture_cache() = 0;

		///
		virtual void draw_rectangle(const RectF& rect) = 0;

		///
		virtual void draw_rectangle(const RectF& rect, const RectF& texture_rect) = 0;

		///
		virtual void draw_text(const Vector2f& position, const StaticString& text,
			unsigned alignment = BottomRightAlignment, TextCapture* capture = nullptr) = 0;

		///
		virtual void set_color(const Vector4f& color) = 0;

		///
		virtual bool set_font(const TextureFont& font) = 0;

		///
		virtual void set_font_size(const Vector2f& size) = 0;

		///
		void set_font_size(float y_size, float x_scaling = 1)
		{
			set_font_size(Vector2f(x_scaling, y_size));
		}

		///
		virtual void set_texture(const Pointer<Texture2D>& texture) = 0;

		///
		virtual bool set_texture_borders(const MarginsI& borders) = 0;

		///
		virtual void set_texture_rectangle(const RectF& rect) = 0;

		///
		void set_texture_rectangle(float x, float y, float width, float height)
		{
			set_texture_rectangle(RectF(x, y, width, height));
		}

		///
		virtual Vector2f text_size(const StaticString& text) const = 0;

		///
		virtual Size window_size() const = 0;
	};
}

#endif // __Y_RENDERER_H
