// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "renderer.h"

#include <yttrium/geometry/matrix.h>
#include <yttrium/geometry/rect.h>
#include <yttrium/image/image.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/program.h>
#include "model/formats/obj.h"
#include "model/mesh_data.h"

#if YTTRIUM_RENDERER_OPENGL
#	include "backend/opengl/renderer.h"
#elif YTTRIUM_RENDERER_VULKAN
#	include "backend/vulkan/renderer.h"
#else
#	include "backend/null/renderer.h"
#endif

#include <cassert>

// TODO: Load textures without intermediate Images.

namespace Yt
{
#if YTTRIUM_RENDERER_OPENGL
	using RenderBackendImpl = GlRenderer;
#elif YTTRIUM_RENDERER_VULKAN
	using RenderBackendImpl = VulkanRenderer;
#else
	using RenderBackendImpl = NullRenderer;
#endif

	RendererImpl::RendererImpl(const WindowID& window_id)
		: _backend{ std::make_unique<RenderBackendImpl>(window_id) }
	{
	}

	RendererImpl::~RendererImpl() = default;

	std::unique_ptr<RenderProgram> RendererImpl::create_program(const std::string& vertex_shader, const std::string& fragment_shader)
	{
		return _backend->create_program(vertex_shader, fragment_shader);
	}

	std::unique_ptr<Texture2D> RendererImpl::create_texture_2d(const Image& image, Flags<TextureFlag> flags)
	{
		return _backend->create_texture_2d(image, flags);
	}

	std::unique_ptr<Mesh> RendererImpl::load_mesh(const Source& source, std::string_view source_name)
	{
		const auto data = load_obj_mesh(source, source_name);
		assert(!data._vertex_format.empty());
		assert(data._vertex_data.size() > 0);
		assert(!data._indices.empty());
		return _backend->create_mesh(data);
	}

	RectF RendererImpl::map_rect(const RectF& rect, ImageOrientation orientation) const
	{
		return _backend->map_rect(rect, orientation);
	}

	void RendererImpl::set_viewport_size(const Size& size)
	{
		_backend->set_viewport_size(size);
	}

	Image RendererImpl::take_screenshot(const Size& size) const
	{
		return _backend->take_screenshot(size);
	}
}
