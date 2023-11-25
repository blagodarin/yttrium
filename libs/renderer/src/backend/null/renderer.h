// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/geometry/rect.h>
#include <yttrium/renderer/mesh.h>
#include "../backend.h"

namespace Yt
{
	struct WindowID;

	class NullRenderer final : public RenderBackend
	{
	public:
		explicit NullRenderer(const WindowID&) {}

		void clear() override {}
		std::unique_ptr<RenderProgram> create_builtin_program_2d() override { return create_program({}, {}); }
		std::unique_ptr<Mesh> create_mesh(const MeshData&) override { return std::make_unique<Mesh>(); }
		std::unique_ptr<RenderProgram> create_program(const std::string&, const std::string&) override;
		std::unique_ptr<Texture2D> create_texture_2d(const seir::ImageInfo&, const void*, Flags<RenderManager::TextureFlag>) override;
		size_t draw_mesh(const Mesh&) override { return 0; }
		void flush_2d(const Buffer&, const Buffer&) noexcept override {}
		RectF map_rect(const RectF& rect, ImageOrientation) const override { return rect; }
		void set_program(const RenderProgram*) override {}
		void set_texture(const Texture2D&, Flags<Texture2D::Filter>) override {}
		void set_viewport_size(const Size&) override {}
		seir::Image take_screenshot(const Size&) const override;
	};
}
