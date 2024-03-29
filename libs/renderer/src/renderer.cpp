// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "renderer.h"

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

#include <seir_graphics/rectf.hpp>
#include <seir_image/image.hpp>

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

	std::unique_ptr<Texture2D> RendererImpl::create_texture_2d(const seir::Image& image, Flags<TextureFlag> flags)
	{
		return _backend->create_texture_2d(image.info(), image.data(), flags);
	}

	std::unique_ptr<Mesh> RendererImpl::load_mesh(const seir::Blob& blob, std::string_view source_name)
	{
		const auto data = load_obj_mesh(blob, source_name);
		assert(!data._vertex_format.empty());
		assert(data._vertex_data.size() > 0);
		assert(!data._indices.empty());
		return _backend->create_mesh(data);
	}

	seir::RectF RendererImpl::map_rect(const seir::RectF& rect, seir::ImageAxes axes) const
	{
		return _backend->map_rect(rect, axes);
	}

	void RendererImpl::set_viewport_size(const seir::Size& size)
	{
		_backend->set_viewport_size(size);
	}

	seir::Image RendererImpl::take_screenshot(const seir::Size& size) const
	{
		return _backend->take_screenshot(size);
	}
}
