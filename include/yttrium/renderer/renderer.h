/// \file
/// \brief

#ifndef _include_yttrium_renderer_renderer_h_
#define _include_yttrium_renderer_renderer_h_

#include <yttrium/flags.h>
#include <yttrium/math/vector4.h>

#include <memory>
#include <vector>

namespace Yttrium
{
	class GpuProgram;
	class Image;
	class IndexBuffer;
	enum class IndexFormat;
	class Margins;
	class Matrix4;
	class Mesh;
	class Point;
	class RectF;
	class Size;
	class Source;
	class Texture2D;
	class TexturedRect;
	enum class VA;
	class Vector3;
	class VertexBuffer;

	///
	/// \note Lifetimes of entities created by a renderer must not exceed the lifetime of the renderer.
	class Renderer
	{
	public:
		///
		enum class TextureFlag
		{
			NoMipmaps = 1 << 0,
			Intensity = 1 << 1,
		};

		///
		virtual ~Renderer() = default;

		///
		virtual std::unique_ptr<GpuProgram> create_gpu_program(const std::string& vertex_shader, const std::string& fragment_shader) = 0;

		///
		virtual std::unique_ptr<IndexBuffer> create_index_buffer(IndexFormat, std::size_t count, const void* data = nullptr) = 0;

		///
		virtual std::unique_ptr<Texture2D> create_texture_2d(Image&&, Flags<TextureFlag> = {}) = 0;

		///
		virtual std::unique_ptr<VertexBuffer> create_vertex_buffer(const std::vector<VA>&, std::size_t count, const void* data = nullptr) = 0;

		///
		virtual void draw_debug_text(const std::string&) = 0;

		///
		virtual void draw_mesh(const Mesh&) = 0;

		///
		virtual void draw_rect(const RectF&, const Vector4& color = { 1, 1, 1, 1 }) = 0;

		///
		virtual void draw_rects(const std::vector<TexturedRect>&, const Vector4& color = { 1, 1, 1, 1 }) = 0;

		///
		virtual void draw_triangles(const VertexBuffer&, const IndexBuffer&) = 0;

		///
		virtual Matrix4 full_matrix() const = 0;

		///
		virtual std::unique_ptr<Mesh> load_mesh(const Source&) = 0;

		///
		virtual Matrix4 model_matrix() const = 0;

		///
		virtual std::pair<Vector3, Vector3> pixel_ray(const Point&) const = 0;

		///
		virtual void set_texture_rect(const RectF&, const Margins&) = 0;

		///
		virtual Size window_size() const = 0;
	};
}

#endif
