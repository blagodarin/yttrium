/// \file
/// \brief

#ifndef _include_yttrium_renderer_h_
#define _include_yttrium_renderer_h_

#include <yttrium/index_buffer.h>
#include <yttrium/rect.h>
#include <yttrium/types.h>

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
		virtual Pointer<IndexBuffer> create_index_buffer(IndexBuffer::Format format, size_t size, const void* data = nullptr) = 0;

		///
		virtual SharedPtr<Texture2D> create_texture_2d(const ImageFormat& format, const void* data, bool no_mipmaps = false) = 0;

		///
		virtual Pointer<VertexBuffer> create_vertex_buffer(unsigned format, size_t size, const void* data = nullptr) = 0;

		///
		virtual Matrix4 current_projection() const = 0;

		///
		virtual Matrix4 current_transformation() const = 0;

		///
		virtual void draw_rectangle(const RectF& rect) = 0;

		///
		virtual void draw_rectangle(const RectF& rect, const RectF& texture_rect) = 0;

		///
		virtual void draw_text(const Vector2& position, const StaticString& text,
			unsigned alignment = BottomRightAlignment, TextCapture* capture = nullptr) = 0;

		///
		virtual void draw_triangles(const VertexBuffer&, const IndexBuffer&) = 0;

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
	class Y_API Push2D
	{
	public:

		///
		Push2D(Renderer& renderer);

		///
		~Push2D();

		Push2D(const Push2D&) = delete;
		Push2D& operator=(const Push2D&) = delete;
	private:
		Renderer& _renderer;
	};

	///
	class Y_API Push3D
	{
	public:

		///
		Push3D(Renderer& renderer, const Matrix4& projection);

		///
		~Push3D();

		Push3D(const Push3D&) = delete;
		Push3D& operator=(const Push3D&) = delete;
	private:
		Renderer& _renderer;
	};

	///
	class Y_API PushGpuProgram
	{
	public:

		///
		PushGpuProgram(Renderer&, const GpuProgram*);

		///
		~PushGpuProgram();

		PushGpuProgram(const PushGpuProgram&) = delete;
		PushGpuProgram& operator=(const PushGpuProgram&) = delete;
	private:
		Renderer& _renderer;
	};

	///
	class Y_API PushTexture
	{
	public:

		///
		PushTexture(Renderer& renderer, const Texture2D* texture);

		///
		~PushTexture();

		PushTexture(const PushTexture&) = delete;
		PushTexture& operator=(const PushTexture&) = delete;
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
		PushTransformation& operator=(const PushTransformation&) = delete;
	private:
		Renderer& _renderer;
	};
}

#endif
