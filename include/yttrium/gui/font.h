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
	class Image;
	class Size;
	class SizeF;
	class Source;
	class TextCapture;
	class TexturedRect;
	class Vector2;

	class Y_ENGINE_API Font
	{
	public:
		explicit Font(const Source&);
		~Font() noexcept;

		void build(std::vector<TexturedRect>&, const Vector2& top_left, float font_size, std::string_view, TextCapture* = nullptr) const;
		Image image() const;
		Size text_size(std::string_view) const;
		SizeF text_size(std::string_view, const SizeF& font_size) const;

	private:
		const std::unique_ptr<class FontPrivate> _private;
	};
}

#endif
