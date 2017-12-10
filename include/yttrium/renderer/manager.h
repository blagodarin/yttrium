#ifndef _include_yttrium_renderer_manager_h_
#define _include_yttrium_renderer_manager_h_

#include <yttrium/flags.h>

#include <memory>
#include <string>

namespace Yttrium
{
	class GpuProgram;
	class Image;
	class Mesh;
	class Source;
	class Texture2D;

	///
	/// \note Lifetimes of entities created by a RenderManager must not exceed the lifetime of the RenderManager.
	class RenderManager
	{
	public:
		///
		enum class TextureFlag
		{
			NoMipmaps = 1 << 0,
			Intensity = 1 << 1,
		};

		///
		virtual ~RenderManager() noexcept = default;

		///
		virtual std::unique_ptr<GpuProgram> create_gpu_program(const std::string& vertex_shader, const std::string& fragment_shader) = 0;

		///
		virtual std::unique_ptr<Texture2D> create_texture_2d(Image&&, Flags<TextureFlag> = {}) = 0;

		///
		virtual std::unique_ptr<Mesh> load_mesh(const Source&) = 0;
	};
}

#endif
