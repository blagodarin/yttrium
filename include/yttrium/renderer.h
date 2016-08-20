/// \file
/// \brief

#ifndef _include_yttrium_renderer_h_
#define _include_yttrium_renderer_h_

#include <cstddef>
#include <initializer_list>

namespace Yttrium
{
	class GpuProgram;
	class ImageFormat;
	class IndexBuffer;
	class Margins;
	class Matrix4;
	class RectF;
	class Size;
	class StaticString;
	class TextGeometry;
	class Texture2D;
	class Vector4;
	class VertexBuffer;

	template <typename> class Pointer;
	template <typename> class SharedPtr;

	enum class IndexFormat;
	enum class VA;

	///
	/// \note Lifetimes of entities created by a renderer must not exceed the lifetime of the renderer.
	class Renderer
	{
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		///
		virtual Pointer<GpuProgram> create_gpu_program(const StaticString& vertex_shader, const StaticString& fragment_shader) = 0;

		///
		virtual Pointer<IndexBuffer> create_index_buffer(IndexFormat, size_t count, const void* data = nullptr) = 0;

		///
		virtual SharedPtr<Texture2D> create_texture_2d(const ImageFormat&, const void* data, bool no_mipmaps = false) = 0;

		///
		virtual Pointer<VertexBuffer> create_vertex_buffer(std::initializer_list<VA>, size_t count, const void* data = nullptr) = 0;

		///
		virtual Matrix4 current_projection() const = 0;

		///
		virtual Matrix4 current_transformation() const = 0;

		///
		virtual void draw_rectangle(const RectF&) = 0;

		///
		virtual void draw_rectangle(const RectF& rect, const RectF& texture_rect) = 0;

		///
		virtual void draw_text(const TextGeometry&) = 0;

		///
		virtual void draw_triangles(const VertexBuffer&, const IndexBuffer&) = 0;

		///
		virtual void set_color(const Vector4&) = 0;

		///
		virtual bool set_texture_borders(const Margins&) = 0;

		///
		virtual void set_texture_rectangle(const RectF&) = 0;

		///
		virtual Size window_size() const = 0;
	};
}

#endif
