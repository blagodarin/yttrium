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

#include "font_data.h"

#include <yttrium/gui/text_capture.h>
#include <yttrium/renderer/textured_rect.h>

namespace Yttrium
{
	void FontData::build(std::vector<TexturedRect>& rects, const Vector2& top_left, float font_size, std::string_view text, TextCapture* capture) const
	{
		rects.clear();

		auto current_x = top_left.x;
		const auto current_y = top_left.y;
		const auto scaling = font_size / static_cast<float>(_size);

		float selection_left = 0;
		const auto do_capture = [font_size, capture, &current_x, current_y, &selection_left](size_t index) {
			if (!capture)
				return;

			if (capture->_cursor_pos == index)
			{
				capture->_cursor_rect = { { current_x, current_y + font_size * 0.125f }, SizeF(2, font_size * 0.75f) };
				capture->_has_cursor = true;
			}

			if (capture->_selection_begin < capture->_selection_end)
			{
				if (index == capture->_selection_begin)
				{
					selection_left = current_x;
				}
				else if (index == capture->_selection_end)
				{
					capture->_selection_rect = { { selection_left, current_y }, Vector2{ current_x, current_y + font_size } };
					capture->_has_selection = true;
				}
			}
		};

		char last_symbol = '\0';
		const char* current_symbol = text.data();

		for (size_t i = 0; i < text.size(); ++i, ++current_symbol)
		{
			const auto info = _chars.find(*current_symbol);
			if (info != _chars.end())
			{
				rects.emplace_back(
					RectF(
						{ current_x + static_cast<float>(info->second.offset._x) * scaling, current_y + static_cast<float>(info->second.offset._y) * scaling },
						SizeF(info->second.rect.size()) * scaling),
					RectF(info->second.rect));
				do_capture(i);
				const auto kerning = _kernings.find(FontData::key(last_symbol, *current_symbol));
				current_x += static_cast<float>(info->second.advance + (kerning != _kernings.end() ? kerning->second : 0)) * scaling;
			}
			last_symbol = *current_symbol;
		}

		do_capture(text.size());
	}

	Size FontData::text_size(std::string_view text) const
	{
		int width = 0;
		for (size_t i = 0; i < text.size(); ++i)
		{
			const auto info = _chars.find(text[i]);
			if (info != _chars.end())
				width += info->second.advance;
			if (i > 0)
			{
				const auto kerning = _kernings.find(FontData::key(text[i - 1], text[i]));
				if (kerning != _kernings.end())
					width += kerning->second;
			}
		}
		return { width, _size };
	}

	SizeF FontData::text_size(std::string_view text, const SizeF& font_size) const
	{
		const SizeF size{ text_size(text) };
		return { font_size._width * (size._width * font_size._height / size._height), font_size._height };
	}
}
