/// \file
/// \brief

#ifndef __Y_RENDERER_H
#define __Y_RENDERER_H

#include <yttrium/index_buffer.h>
#include <yttrium/rect.h>
#include <yttrium/types.h>

#include <memory>

namespace Yttrium
{
	class Margins;
	class Matrix4;
	class StaticString;
	class Texture2D;
	class TextureCache;
	class TextureFont;
	class Vector4;
	class VertexBuffer;

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
		virtual std::unique_ptr<IndexBuffer> create_index_buffer(IndexBuffer::Format format, size_t size) = 0;

		///
		/// \note Texture cache lifetime must not exceed its renderer lifetime.
		virtual std::unique_ptr<TextureCache> create_texture_cache() = 0;

		///
		virtual std::unique_ptr<VertexBuffer> create_vertex_buffer(unsigned format, size_t size) = 0;

		// NOTE: Temporary!
		virtual void draw_cube(const Vector4& center, float size) = 0;

		///
		virtual void draw_rectangle(const RectF& rect) = 0;

		///
		virtual void draw_rectangle(const RectF& rect, const RectF& texture_rect) = 0;

		///
		virtual void draw_text(const Vector2& position, const StaticString& text,
			unsigned alignment = BottomRightAlignment, TextCapture* capture = nullptr) = 0;

		///
		virtual void set_color(const Vector4& color) = 0;

		///
		virtual bool set_font(const TextureFont& font) = 0;

		///
		virtual void set_font_size(const Vector2& size) = 0;

		///
		void set_font_size(float y_size, float x_scaling = 1)
		{
			set_font_size(Vector2(x_scaling, y_size));
		}

		///
		virtual void set_texture(const Pointer<Texture2D>& texture) = 0;

		///
		virtual bool set_texture_borders(const Margins& borders) = 0;

		///
		virtual void set_texture_rectangle(const RectF& rect) = 0;

		///
		void set_texture_rectangle(float x, float y, float width, float height)
		{
			set_texture_rectangle(RectF(x, y, width, height));
		}

		///
		virtual Vector2 text_size(const StaticString& text) const = 0;

		///
		virtual Size window_size() const = 0;
	};

	///
	class Y_API PushProjection
	{
	public:

		///
		PushProjection(Renderer& renderer, const Matrix4& matrix);

		~PushProjection();

		PushProjection(const PushProjection&) = delete;
		PushProjection(PushProjection&&) = delete;
		PushProjection& operator=(const PushProjection&) = delete;
		PushProjection& operator=(PushProjection&&) = delete;

	private:

		Renderer& _renderer;
	};

	///
	class Y_API PushTransformation
	{
	public:

		/// Multiplies the current transformation matrix by the specified one,
		/// pushes it to the transformation stack and applies the resulting transformation.
		PushTransformation(Renderer& renderer, const Matrix4& matrix);

		/// Pops a matrix from the transformation stack and applies the next matrix.
		~PushTransformation();

		PushTransformation(const PushTransformation&) = delete;
		PushTransformation(PushTransformation&&) = delete;
		PushTransformation& operator=(const PushTransformation&) = delete;
		PushTransformation& operator=(PushTransformation&&) = delete;

	private:

		Renderer& _renderer;
	};
}

#endif // __Y_RENDERER_H
