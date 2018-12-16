//
// Copyright 2018 Sergei Blagodarin
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

#ifndef _include_yttrium_gui_font_h_
#define _include_yttrium_gui_font_h_

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
		static std::unique_ptr<Font> load(const Source&, RenderManager&);
		static std::unique_ptr<Font> load(const Source&, const std::shared_ptr<const Texture2D>&);

		virtual ~Font() noexcept = default;
		virtual void build(std::vector<TexturedRect>&, const Vector2& top_left, float font_size, std::string_view, TextCapture* = nullptr) const = 0;
		virtual Size text_size(std::string_view) const = 0;
		virtual SizeF text_size(std::string_view, const SizeF& font_size) const = 0;
		virtual const Texture2D* texture() const = 0;
	};
}

#endif
