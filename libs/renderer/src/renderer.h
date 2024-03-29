// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/renderer/manager.h>

#include <memory>

namespace seir
{
	enum class ImageAxes;
	class RectF;
	class Size;
}

namespace Yt
{
	enum class ImageOrientation;
	class RenderBackend;
	struct WindowID;

	class RendererImpl final : public RenderManager
	{
	public:
		explicit RendererImpl(const WindowID&);
		~RendererImpl() override;

		std::unique_ptr<RenderProgram> create_program(const std::string& vertex_shader, const std::string& fragment_shader) override;
		std::unique_ptr<Texture2D> create_texture_2d(const seir::Image&, Flags<TextureFlag>) override;
		std::unique_ptr<Mesh> load_mesh(const seir::Blob&, std::string_view source_name) override;

	public:
		seir::RectF map_rect(const seir::RectF&, seir::ImageAxes) const;
		void set_viewport_size(const seir::Size&);
		seir::Image take_screenshot(const seir::Size&) const;

	public:
		const std::unique_ptr<RenderBackend> _backend;
	};
}
