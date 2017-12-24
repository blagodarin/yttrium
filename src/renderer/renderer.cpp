#include "renderer.h"

#include <yttrium/image.h>
#include <yttrium/math/matrix.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/program.h>
#include "formats/obj.h"
#include "mesh_data.h"

#include "../config.h"
#if Y_RENDERER_OPENGL
	#include "gl/renderer.h"
#elif Y_RENDERER_VULKAN
	#include "vulkan/renderer.h"
#else
	#include "null/renderer.h"
#endif

#include <cassert>

namespace Yttrium
{
	RendererImpl::RendererImpl(WindowBackend& window)
#if Y_RENDERER_OPENGL
		: _backend{std::make_unique<GlRenderer>(window)}
#elif Y_RENDERER_VULKAN
		: _backend{std::make_unique<VulkanRenderer>(window)}
#else
		: _backend{std::make_unique<NullRenderer>(window)}
#endif
	{
	}

	RendererImpl::~RendererImpl() = default;

	std::unique_ptr<RenderProgram> RendererImpl::create_program(const std::string& vertex_shader, const std::string& fragment_shader)
	{
		return _backend->create_program(vertex_shader, fragment_shader);
	}

	std::unique_ptr<Texture2D> RendererImpl::create_texture_2d(Image&& image, Flags<TextureFlag> flags)
	{
		return _backend->create_texture_2d(std::move(image), flags);
	}

	std::unique_ptr<Mesh> RendererImpl::load_mesh(const Source& source)
	{
		const auto data = load_obj_mesh(source);
		assert(!data._vertex_format.empty());
		assert(data._vertex_data.size() > 0);
		assert(!data._indices.empty());
		return _backend->create_mesh(data);
	}

	RectF RendererImpl::map_rect(const RectF& rect, ImageOrientation orientation) const
	{
		return _backend->map_rect(rect, orientation);
	}

	void RendererImpl::set_window_size(const Size& size)
	{
		_backend->set_window_size(size);
	}

	Image RendererImpl::take_screenshot(const Size& size) const
	{
		return _backend->take_screenshot(size);
	}
}
