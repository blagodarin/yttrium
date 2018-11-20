//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "renderer.h"

#include <yttrium/image.h>
#include <yttrium/math/matrix.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/program.h>
#include "formats/obj.h"
#include "mesh_data.h"

#if Y_RENDERER_OPENGL
#	include "_opengl/renderer.h"
#elif Y_RENDERER_VULKAN
#	include "_vulkan/renderer.h"
#else
#	include "_null/renderer.h"
#endif

#include <cassert>

namespace Yttrium
{
#if Y_RENDERER_OPENGL
	using RenderBackendImpl = GlRenderer;
#elif Y_RENDERER_VULKAN
	using RenderBackendImpl = VulkanRenderer;
#else
	using RenderBackendImpl = NullRenderer;
#endif

	RendererImpl::RendererImpl(WindowBackend& window)
		: _backend{ std::make_unique<RenderBackendImpl>(window) }
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
