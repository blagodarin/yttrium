#ifndef _src_renderer_renderer_h_
#define _src_renderer_renderer_h_

#include <yttrium/renderer/manager.h>

#include <yttrium/memory/buffer.h>

#include <memory>
#include <vector>

namespace Yttrium
{
	class BackendTexture2D;
	enum class ImageOrientation;
	class Matrix4;
	class RectF;
	class RendererBackend;
	class Size;
	class WindowBackend;

	enum class RenderMatrixType
	{
		Projection,
		View,
		Model,
	};

	class RendererImpl final : public RenderManager
	{
	public:
		explicit RendererImpl(WindowBackend&);
		~RendererImpl() override;

		std::unique_ptr<GpuProgram> create_gpu_program(const std::string& vertex_shader, const std::string& fragment_shader) override;
		std::unique_ptr<Texture2D> create_texture_2d(Image&&, Flags<TextureFlag>) override;
		std::unique_ptr<Mesh> load_mesh(const Source&) override;

	public:
		const Texture2D* debug_texture() const { return _debug_texture.get(); }
		RectF map_rect(const RectF&, ImageOrientation) const;
		const GpuProgram* program_2d() const { return _program_2d.get(); }
		void set_window_size(const Size&);
		Image take_screenshot(const Size&) const;

	private:
		const std::unique_ptr<RendererBackend> _backend;

		Buffer _vertices_2d;
		Buffer _indices_2d;

		std::unique_ptr<const Texture2D> _white_texture;
		std::unique_ptr<const Texture2D> _debug_texture;
		std::unique_ptr<GpuProgram> _program_2d;

		std::vector<std::pair<Matrix4, RenderMatrixType>> _matrix_stack;

		std::vector<std::pair<const Texture2D*, int>> _texture_stack{ { nullptr, 1 } };
#ifndef NDEBUG
		std::vector<const Texture2D*> _seen_textures; // For redundancy statistics.
#endif

		std::vector<std::pair<const GpuProgram*, int>> _program_stack{ { nullptr, 1 } };
#ifndef NDEBUG
		std::vector<const GpuProgram*> _seen_programs; // For redundancy statistics.
#endif

		std::string _debug_text;

		friend class RenderContextImpl;
	};
}

#endif
