/// \file
/// \brief

#ifndef _include_yttrium_renderer_h_
#define _include_yttrium_renderer_h_

#include <yttrium/std/vector.h>

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
	class Texture2D;
	class TexturedRect;
	class Vector4;
	class VertexBuffer;

	template <typename> class UniquePtr;

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
		virtual UniquePtr<GpuProgram> create_gpu_program(const StaticString& vertex_shader, const StaticString& fragment_shader) = 0;

		///
		virtual UniquePtr<IndexBuffer> create_index_buffer(IndexFormat, size_t count, const void* data = nullptr) = 0;

		///
		virtual UniquePtr<Texture2D> create_texture_2d(const ImageFormat&, const void* data, bool no_mipmaps = false) = 0;

		///
		virtual UniquePtr<VertexBuffer> create_vertex_buffer(std::initializer_list<VA>, size_t count, const void* data = nullptr) = 0;

		///
		virtual Matrix4 current_projection() const = 0;

		///
		virtual Matrix4 current_transformation() const = 0;

		///
		virtual void draw_rect(const RectF&) = 0;

		///
		virtual void draw_rects(const StdVector<TexturedRect>&) = 0;

		///
		virtual void draw_triangles(const VertexBuffer&, const IndexBuffer&) = 0;

		///
		virtual void set_color(const Vector4&) = 0;

		///
		virtual void set_texture_rect(const RectF&, const Margins&) = 0;

		///
		virtual Size window_size() const = 0;
	};
}

#endif
