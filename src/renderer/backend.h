#ifndef _src_renderer_backend_h_
#define _src_renderer_backend_h_

#include <yttrium/math/color.h>
#include <yttrium/math/vector.h>
#include <yttrium/renderer/manager.h>
#include <yttrium/renderer/texture.h>

namespace Yttrium
{
	class Buffer;
	enum class ImageOrientation;
	class MeshData;
	class RectF;
	class RendererImpl;

	class RendererBackend
	{
	public:
		struct Vertex2D
		{
			Vector2 position;
			Color4f color;
			Vector2 texture;
		};

		virtual ~RendererBackend() noexcept = default;

		virtual void clear() = 0;
		virtual std::unique_ptr<RenderProgram> create_builtin_program_2d(RendererImpl&) = 0;
		virtual std::unique_ptr<Mesh> create_mesh(const MeshData&) = 0;
		virtual std::unique_ptr<RenderProgram> create_program(RendererImpl&, const std::string& vertex_shader, const std::string& fragment_shader) = 0;
		virtual std::unique_ptr<Texture2D> create_texture_2d(RendererImpl&, Image&&, Flags<RenderManager::TextureFlag>) = 0;
		virtual size_t draw_mesh(const Mesh&) = 0;
		virtual void flush_2d(const Buffer& vertices, const Buffer& indices) = 0;
		virtual RectF map_rect(const RectF&, ImageOrientation) const = 0;
		virtual void set_program(const RenderProgram*) = 0;
		virtual void set_texture(const Texture2D&, Flags<Texture2D::Filter>) = 0;
		virtual void set_window_size(const Size&) = 0;
		virtual Image take_screenshot(const Size&) const = 0;
	};
}

#endif
