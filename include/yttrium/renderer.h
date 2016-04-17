/// \file
/// \brief

#ifndef _include_yttrium_renderer_h_
#define _include_yttrium_renderer_h_

#include <yttrium/index_buffer.h>
#include <yttrium/math/rect.h>

#include <initializer_list>

namespace Yttrium
{
	class GpuProgram;
	class ImageFormat;
	class Margins;
	class Matrix4;
	class StaticString;
	class Texture2D;
	class TextureFont;
	class Vector4;
	class VertexBuffer;

	template <typename> class Pointer;
	template <typename> class SharedPtr;

	enum class VA;

	///
	/// \note Lifetimes of entities created by a renderer must not exceed the lifetime of the renderer.
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
		virtual Pointer<GpuProgram> create_gpu_program(const StaticString& vertex_shader, const StaticString& fragment_shader) = 0;

		///
		virtual Pointer<IndexBuffer> create_index_buffer(IndexBuffer::Format format, size_t count, const void* data = nullptr) = 0;

		///
		virtual SharedPtr<Texture2D> create_texture_2d(const ImageFormat& format, const void* data, bool no_mipmaps = false) = 0;

		///
		virtual Pointer<VertexBuffer> create_vertex_buffer(std::initializer_list<VA>, size_t count, const void* data = nullptr) = 0;

		///
		virtual Matrix4 current_projection() const = 0;

		///
		virtual Matrix4 current_transformation() const = 0;

		///
		virtual void draw_rectangle(const RectF& rect) = 0;

		///
		virtual void draw_rectangle(const RectF& rect, const RectF& texture_rect) = 0;

		///
		virtual void draw_text(const PointF& top_left, float font_size, const StaticString& text, TextCapture* capture = nullptr) = 0;

		///
		virtual void draw_triangles(const VertexBuffer&, const IndexBuffer&) = 0;

		///
		virtual void set_color(const Vector4& color) = 0;

		///
		virtual bool set_font(const TextureFont& font) = 0;

		///
		virtual bool set_texture_borders(const Margins& borders) = 0;

		///
		virtual void set_texture_rectangle(const RectF& rect) = 0;

		///
		virtual Size window_size() const = 0;
	};
}

#endif
