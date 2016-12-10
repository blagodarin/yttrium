/// \file
/// \brief

#ifndef _include_yttrium_renderer_renderer_h_
#define _include_yttrium_renderer_renderer_h_

#include <yttrium/math/vector.h>
#include <yttrium/std/vector.h>

namespace Yttrium
{
	class GpuProgram;
	class Image;
	class IndexBuffer;
	enum class IndexFormat;
	class Margins;
	class Matrix4;
	class Mesh;
	class Reader;
	class RectF;
	template <typename> class ResourcePtr;
	class StaticString;
	class Texture2D;
	class TexturedRect;
	template <typename> class UniquePtr;
	enum class VA;
	class VertexBuffer;

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
		virtual ResourcePtr<Texture2D> create_texture_2d(const Image&, bool no_mipmaps = false) = 0;

		///
		virtual UniquePtr<VertexBuffer> create_vertex_buffer(std::initializer_list<VA>, size_t count, const void* data = nullptr) = 0;

		///
		virtual void draw_debug_text(const StaticString&) = 0;

		///
		virtual void draw_mesh(const Mesh&) = 0;

		///
		virtual void draw_rect(const RectF&, const Vector4& color = { 1, 1, 1, 1 }) = 0;

		///
		virtual void draw_rects(const StdVector<TexturedRect>&, const Vector4& color = { 1, 1, 1, 1 }) = 0;

		///
		virtual void draw_triangles(const VertexBuffer&, const IndexBuffer&) = 0;

		///
		virtual Matrix4 full_matrix() const = 0;

		///
		virtual ResourcePtr<Mesh> load_mesh(Reader&&) = 0;

		///
		virtual Matrix4 model_matrix() const = 0;

		///
		virtual void set_texture_rect(const RectF&, const Margins&) = 0;

		///
		virtual Size window_size() const = 0;
	};
}

#endif
