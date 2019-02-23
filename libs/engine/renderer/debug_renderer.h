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
#include <yttrium/renderer/modifiers.h>

#include <optional>
#include <string_view>

namespace Yttrium
{
	class RenderPassImpl;

	class DebugRenderer
	{
	public:
		explicit DebugRenderer(RenderPass&);

		void draw_cursor(size_t x, size_t y);
		void draw_rectangle(size_t x, size_t y, size_t width, size_t height);
		void draw_text(size_t x, size_t y, std::string_view text, const std::optional<size_t>& max_size = {});
		size_t max_width() const;
		void set_color(float r, float g, float b, float a = 1);

	private:
		RenderPassImpl& _pass;
		Color4f _color{ 1, 1, 1 };
		PushTexture _texture;
	};
}
