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

#include <yttrium/api.h>

#include <memory>
#include <string_view>
#include <vector>

namespace Yttrium
{
	class RenderManager;
	class Size;
	class SizeF;
	class Source;
	class TextCapture;
	class Texture2D;
	class TexturedRect;
	class Vector2;

	class Y_ENGINE_API Font
	{
	public:
		struct Graphics
		{
			std::shared_ptr<const Texture2D> _texture;
			std::vector<TexturedRect> _glyphs;
		};

		static std::unique_ptr<Font> load(const Source&, RenderManager&);

		virtual ~Font() noexcept = default;
		virtual void build(Graphics&, const Vector2& top_left, float font_size, std::string_view, TextCapture* = nullptr) const = 0;
		virtual Size text_size(std::string_view) const = 0;
		virtual SizeF text_size(std::string_view, const SizeF& font_size) const = 0;
	};
}
