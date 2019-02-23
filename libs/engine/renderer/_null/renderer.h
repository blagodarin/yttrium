//
// Copyright 2019 Sergei Blagodarin
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

#pragma once

#include <yttrium/math/rect.h>
#include <yttrium/renderer/mesh.h>
#include "../backend.h"

namespace Yttrium
{
	class WindowBackend;

	class NullRenderer final : public RenderBackend
	{
	public:
		explicit NullRenderer(const WindowBackend&) {}

		void clear() override {}
		std::unique_ptr<RenderProgram> create_builtin_program_2d() override { return create_program({}, {}); }
		std::unique_ptr<Mesh> create_mesh(const MeshData&) override { return std::make_unique<Mesh>(); }
		std::unique_ptr<RenderProgram> create_program(const std::string&, const std::string&) override;
		std::unique_ptr<Texture2D> create_texture_2d(const Image&, Flags<RenderManager::TextureFlag>) override;
		size_t draw_mesh(const Mesh&) override { return 0; }
		void flush_2d(const Buffer&, const Buffer&) noexcept override {}
		RectF map_rect(const RectF& rect, ImageOrientation) const override { return rect; }
		void set_program(const RenderProgram*) override {}
		void set_texture(const Texture2D&, Flags<Texture2D::Filter>) override {}
		void set_window_size(const Size&) override {}
		Image take_screenshot(const Size&) const override;
	};
}
