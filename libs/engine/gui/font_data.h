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

#pragma once

#include <yttrium/math/rect.h>

#include <unordered_map>
#include <vector>

namespace Yttrium
{
	class TextCapture;
	class TexturedRect;
	class Vector2;

	struct FontChar
	{
		Rect rect;
		Point offset;
		int advance = 0;
	};

	struct FontData
	{
		const int _size;
		std::unordered_map<char, FontChar> _chars;
		std::unordered_map<std::uint16_t, int> _kernings;

		explicit FontData(int size) noexcept
			: _size{ size } {}
		FontData(FontData&& other) noexcept
			: _size{ other._size }, _chars{ std::move(other._chars) }, _kernings{ std::move(other._kernings) } {}

		void build(std::vector<TexturedRect>&, const Vector2& top_left, float font_size, std::string_view, TextCapture* = nullptr) const;
		Size text_size(std::string_view) const;
		SizeF text_size(std::string_view, const SizeF& font_size) const;

		static constexpr auto key(char a, char b) noexcept
		{
			static_assert(2 * sizeof(char) == sizeof(uint16_t));
			return static_cast<std::uint16_t>(static_cast<unsigned char>(a) + (static_cast<unsigned char>(b) << 8));
		}
	};
}
