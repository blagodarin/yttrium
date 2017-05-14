/// \file
/// \brief

#ifndef _include_yttrium_renderer_renderer_h_
#define _include_yttrium_renderer_renderer_h_

#include <yttrium/flags.h>
#include <yttrium/math/color.h>
#include <yttrium/std/string_view.h>

#include <memory>
#include <vector>

namespace Yttrium
{
	class GpuProgram;
	class Image;
	class IndexBuffer;
	enum class IndexFormat;
	class Line3;
	class Margins;
	class Matrix4;
	class Mesh;
	class Quad;
	class RectF;
	class Size;
	class Source;
	class Texture2D;
	class TexturedRect;
	enum class VA;
	class Vector2;
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
		virtual void add_debug_text(std::string_view) = 0;

		///
		virtual std::unique_ptr<GpuProgram> create_gpu_program(const std::string& vertex_shader, const std::string& fragment_shader) = 0;

		///
		virtual std::unique_ptr<IndexBuffer> create_index_buffer(IndexFormat, std::size_t count, const void* data = nullptr) = 0;

		///
		virtual std::unique_ptr<Texture2D> create_texture_2d(Image&&, Flags<TextureFlag> = {}) = 0;

		///
		virtual std::unique_ptr<VertexBuffer> create_vertex_buffer(const std::vector<VA>&, std::size_t count, const void* data = nullptr) = 0;

		///
		virtual void draw_mesh(const Mesh&) = 0;

		///
		virtual void draw_quad(const Quad&, const Color4f& = {1, 1, 1}) = 0;

		///
		virtual void draw_rect(const RectF&, const Color4f& = {1, 1, 1}) = 0;

		///
		virtual void draw_rects(const std::vector<TexturedRect>&, const Color4f& = {1, 1, 1}) = 0;

		///
		virtual void draw_triangles(const VertexBuffer&, const IndexBuffer&) = 0;

		///
		virtual Matrix4 full_matrix() const = 0;

		///
		virtual std::unique_ptr<Mesh> load_mesh(const Source&) = 0;

		///
		virtual Matrix4 model_matrix() const = 0;

		///
		virtual Line3 pixel_ray(const Vector2&) const = 0;

		///
		virtual void set_texture_rect(const RectF&, const Margins&) = 0;

		///
		virtual Size window_size() const = 0;
	};
}

#endif
