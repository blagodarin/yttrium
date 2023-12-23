// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/renderer/manager.h>
#include <yttrium/renderer/texture.h>

namespace seir
{
	enum class ImageAxes;
	class ImageInfo;
	class RectF;
	class Size;
}

namespace Yt
{
	class Buffer;
	class MeshData;

	class RenderBackend
	{
	public:
		virtual ~RenderBackend() noexcept = default;

		virtual void clear() = 0;
		virtual std::unique_ptr<RenderProgram> create_builtin_program_2d() = 0;
		virtual std::unique_ptr<Mesh> create_mesh(const MeshData&) = 0;
		virtual std::unique_ptr<RenderProgram> create_program(const std::string& vertex_shader, const std::string& fragment_shader) = 0;
		virtual std::unique_ptr<Texture2D> create_texture_2d(const seir::ImageInfo&, const void*, Flags<RenderManager::TextureFlag>) = 0;
		virtual size_t draw_mesh(const Mesh&) = 0;
		virtual void flush_2d(const Buffer& vertices, const Buffer& indices) noexcept = 0;
		virtual seir::RectF map_rect(const seir::RectF&, seir::ImageAxes) const = 0;
		virtual void set_program(const RenderProgram*) = 0;
		virtual void set_texture(const Texture2D&, Flags<Texture2D::Filter>) = 0;
		virtual void set_viewport_size(const seir::Size&) = 0;
		virtual seir::Image take_screenshot(const seir::Size&) const = 0;
	};
}
