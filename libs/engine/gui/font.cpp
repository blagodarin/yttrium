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

#include <yttrium/gui/font.h>

#include <yttrium/exceptions.h>
#include <yttrium/image.h>
#include <yttrium/math/size.h>
#include <yttrium/storage/source.h>
#include "font_data.h"

#include <cassert>
#include <cstring>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Yttrium
{
	class FontPrivate
	{
	public:
		FontPrivate()
		{
			if (FT_Init_FreeType(&_freetype))
				throw InitializationError{ "Failed to initialize FreeType library" };
		}

		~FontPrivate() noexcept
		{
			if (_font_face)
				FT_Done_Face(_font_face); // TODO: Handle error code.
			FT_Done_FreeType(_freetype);  // TODO: Handle error code.
		}

		void load(Buffer&& buffer)
		{
			assert(!_source_buffer.capacity());
			if (buffer.size() > std::numeric_limits<FT_Long>::max()
				|| FT_New_Memory_Face(_freetype, static_cast<const FT_Byte*>(buffer.data()), static_cast<FT_Long>(buffer.size()), 0, &_font_face))
				throw DataError{ "Failed to load font" };
			_source_buffer = std::move(buffer);
		}

		void render()
		{
			assert(_font_face);
			assert(_data._chars.empty());
			const ImageFormat image_format{ static_cast<std::size_t>(_data._size) * 16, static_cast<std::size_t>(_data._size) * 8, PixelFormat::Gray8 };
			Image image{ image_format };
			std::unordered_map<char, FontChar> chars;
			FT_Set_Pixel_Sizes(_font_face, 0, static_cast<FT_UInt>(_data._size));
			std::size_t x_offset = 0;
			std::size_t y_offset = 0;
			for (FT_UInt char_code = 0; char_code < 128; ++char_code)
			{
				if (FT_Load_Char(_font_face, char_code, FT_LOAD_RENDER))
					continue; // TODO: Report error.
				if (_font_face->glyph->bitmap.width > image_format.width() - x_offset)
				{
					x_offset = 0;
					y_offset += static_cast<std::size_t>(_data._size);
				}
				if (_font_face->glyph->bitmap.rows > image_format.height() - y_offset)
					break; // TODO: Report error.
				auto src = _font_face->glyph->bitmap.buffer;
				auto dst = static_cast<std::uint8_t*>(image.data()) + image_format.row_size() * y_offset + x_offset;
				for (unsigned y = 0; y < _font_face->glyph->bitmap.rows; ++y)
				{
					std::memcpy(dst, src, _font_face->glyph->bitmap.width);
					src += _font_face->glyph->bitmap.width;
					dst += image_format.row_size();
				}
				auto font_char = chars[static_cast<char>(char_code)];
				font_char.rect = { { static_cast<int>(x_offset), static_cast<int>(y_offset) }, Size{ static_cast<int>(_font_face->glyph->bitmap.width), static_cast<int>(_font_face->glyph->bitmap.rows) } };
				font_char.offset = { 0, 0 };
				font_char.advance = font_char.rect.width();
			}
			_data._chars = std::move(chars);
			_image = std::move(image);
		}

	private:
		FT_Library _freetype = nullptr;
		Buffer _source_buffer;
		FT_Face _font_face = nullptr;
		FontData _data{ 32 };
		Image _image{ { 0, 0, PixelFormat::Gray8 } };

		friend Font;
	};

	Font::Font(const Source& source)
		: _private{ std::make_unique<FontPrivate>() }
	{
		_private->load(source.to_buffer());
		_private->render();
	}

	Font::~Font() noexcept = default;

	void Font::build(std::vector<TexturedRect>& rects, const Vector2& top_left, float font_size, std::string_view text, TextCapture* capture) const
	{
		_private->_data.build(rects, top_left, font_size, text, capture);
	}

	const Image& Font::image() const
	{
		return _private->_image;
	}

	Size Font::text_size(std::string_view text) const
	{
		return _private->_data.text_size(text);
	}

	SizeF Font::text_size(std::string_view text, const SizeF& font_size) const
	{
		return _private->_data.text_size(text, font_size);
	}
}
