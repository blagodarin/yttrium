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

#include <yttrium/math/color.h>
#include <yttrium/math/vector.h>
#include <yttrium/renderer/manager.h>
#include <yttrium/renderer/texture.h>

namespace Yttrium
{
	class Buffer;
	enum class ImageOrientation;
	class MeshData;
	class RectF;

	class RenderBackend
	{
	public:
		struct Vertex2D
		{
			Vector2 position;
			Color4f color;
			Vector2 texture;
		};

		virtual ~RenderBackend() noexcept = default;

		virtual void clear() = 0;
		virtual std::unique_ptr<RenderProgram> create_builtin_program_2d() = 0;
		virtual std::unique_ptr<Mesh> create_mesh(const MeshData&) = 0;
		virtual std::unique_ptr<RenderProgram> create_program(const std::string& vertex_shader, const std::string& fragment_shader) = 0;
		virtual std::unique_ptr<Texture2D> create_texture_2d(const Image&, Flags<RenderManager::TextureFlag>) = 0;
		virtual size_t draw_mesh(const Mesh&) = 0;
		virtual void flush_2d(const Buffer& vertices, const Buffer& indices) noexcept = 0;
		virtual RectF map_rect(const RectF&, ImageOrientation) const = 0;
		virtual void set_program(const RenderProgram*) = 0;
		virtual void set_texture(const Texture2D&, Flags<Texture2D::Filter>) = 0;
		virtual void set_window_size(const Size&) = 0;
		virtual Image take_screenshot(const Size&) const = 0;
	};
}
