// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/renderer/manager.h>

#include <memory>

namespace Yt
{
	enum class ImageOrientation;
	class RectF;
	class RenderBackend;
	class Size;
	struct WindowID;

	class RendererImpl final : public RenderManager
	{
	public:
		explicit RendererImpl(const WindowID&);
		~RendererImpl() override;

		std::unique_ptr<RenderProgram> create_program(const std::string& vertex_shader, const std::string& fragment_shader) override;
		std::unique_ptr<Texture2D> create_texture_2d(const Image&, Flags<TextureFlag>) override;
		std::unique_ptr<Mesh> load_mesh(const Source&, std::string_view source_name) override;

	public:
		RectF map_rect(const RectF&, ImageOrientation) const;
		void set_window_size(const Size&);
		Image take_screenshot(const Size&) const;

	public:
		const std::unique_ptr<RenderBackend> _backend;
	};
}
