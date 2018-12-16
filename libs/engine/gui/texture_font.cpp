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

#include <yttrium/gui/texture_font.h>

#include <yttrium/exceptions.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include "font_data.h"
#include "texture_font.h"

namespace Yttrium
{
	class TextureFontImpl final : public TextureFont
	{
	public:
		explicit TextureFontImpl(int size)
			: _data{ size } {}

		void build(std::vector<TexturedRect>& rects, const Vector2& top_left, float font_size, std::string_view text, TextCapture* capture) const override
		{
			_data.build(rects, top_left, font_size, text, capture);
		}

		Rect rect() const override
		{
			return _rect;
		}

		Size text_size(std::string_view text) const override
		{
			return _data.text_size(text);
		}

		SizeF text_size(std::string_view text, const SizeF& font_size) const override
		{
			return _data.text_size(text, font_size);
		}

	private:
		FontData _data;
		Rect _rect;

		friend TextureFont;
	};

	std::unique_ptr<TextureFont> TextureFont::load(const Source& source)
	{
		Reader reader{ source };

		uint32_t fourcc = 0;
		if (!reader.read(fourcc) || fourcc != FourccYtf1)
			throw DataError("Bad 'YTF1' fourcc");

		if (!reader.read(fourcc) || fourcc != FourccFont)
			throw DataError("Bad 'font' section fourcc");

		Ytf1Font font_section;
		if (!reader.read(font_section))
			throw DataError("Bad 'font' section");

		if (!reader.read(fourcc) || fourcc != FourccChar)
			throw DataError("Bad 'char' section fourcc");

		uint8_t char_count = 0;
		if (!reader.read(char_count))
			throw DataError("Bad 'char' section header");

		auto font = std::make_unique<TextureFontImpl>(font_section.size);

		Size font_rect_size;
		for (uint8_t i = 0; i < char_count; ++i)
		{
			Ytf1Char char_data;
			if (!reader.read(char_data))
				throw DataError("Bad 'char' section entry ", i);

			FontChar info;
			info.rect = { { font_section.base_x + char_data.x, font_section.base_y + char_data.y }, Size{ char_data.width, char_data.height } };
			info.offset = { char_data.x_offset, char_data.y_offset };
			info.advance = char_data.advance;

			font->_data._chars[char_data.id] = info;
			font_rect_size = { std::max(font_rect_size._width, char_data.x + char_data.width), std::max(font_rect_size._height, char_data.y + char_data.height) };
		}
		font->_rect = { { font_section.base_x, font_section.base_y }, font_rect_size };

		if (reader.read(fourcc))
		{
			if (fourcc != FourccKern)
				throw DataError("Bad 'kern' section fourcc");

			uint16_t kerning_count = 0;
			if (!reader.read(kerning_count))
				throw DataError("Bad 'kern' section header");

			for (uint16_t i = 0; i < kerning_count; ++i)
			{
				Ytf1Kerning kerning;
				if (!reader.read(kerning))
					throw DataError("Bad 'kern' section entry ", i);
				font->_data._kernings.emplace(FontData::key(kerning.first, kerning.second), kerning.amount);
			}
		}

		return std::move(font);
	}
}
