//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include <yttrium/renderer/manager.h>

#include <memory>

namespace Yttrium
{
	enum class ImageOrientation;
	class RectF;
	class RenderBackend;
	class Size;
	class WindowBackend;

	class RendererImpl final : public RenderManager
	{
	public:
		explicit RendererImpl(WindowBackend&);
		~RendererImpl() override;

		std::unique_ptr<RenderProgram> create_program(const std::string& vertex_shader, const std::string& fragment_shader) override;
		std::unique_ptr<Texture2D> create_texture_2d(const Image&, Flags<TextureFlag>) override;
		std::unique_ptr<Mesh> load_mesh(const Source&) override;

	public:
		RectF map_rect(const RectF&, ImageOrientation) const;
		void set_window_size(const Size&);
		Image take_screenshot(const Size&) const;

	public:
		const std::unique_ptr<RenderBackend> _backend;
	};
}
