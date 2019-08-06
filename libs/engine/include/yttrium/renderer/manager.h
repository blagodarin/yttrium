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

#include <yttrium/utils/flags.h>

#include <memory>
#include <string>

namespace Yt
{
	class Image;
	class Mesh;
	class RenderProgram;
	class Source;
	class Texture2D;

	///
	/// \note Lifetimes of entities created by a RenderManager must not exceed the lifetime of the RenderManager.
	class RenderManager
	{
	public:
		///
		enum class TextureFlag
		{
			NoMipmaps = 1 << 0,
		};

		///
		virtual ~RenderManager() noexcept = default;

		///
		virtual std::unique_ptr<RenderProgram> create_program(const std::string& vertex_shader, const std::string& fragment_shader) = 0;

		///
		virtual std::unique_ptr<Texture2D> create_texture_2d(const Image&, Flags<TextureFlag> = {}) = 0;

		///
		virtual std::unique_ptr<Mesh> load_mesh(const Source&, std::string_view source_name) = 0;
	};
}
