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
#include <yttrium/gui/text_capture.h>
#include <yttrium/image.h>
#include <yttrium/math/size.h>
#include <yttrium/renderer/manager.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/renderer/textured_rect.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>

#include <cassert>
#include <cstring>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Yttrium
{
	struct FreeTypeWrapper
	{
		FT_Library _library = nullptr;
		Buffer _face_buffer;
		FT_Face _face = nullptr;

		FreeTypeWrapper()
		{
			if (FT_Init_FreeType(&_library))
				throw InitializationError{ "Failed to initialize FreeType library" };
		}

		~FreeTypeWrapper() noexcept
		{
			if (_face)
				FT_Done_Face(_face);    // TODO: Handle error code.
			FT_Done_FreeType(_library); // TODO: Handle error code.
		}

		void load(Buffer&& buffer)
		{
			assert(!_face);
			if (buffer.size() > std::numeric_limits<FT_Long>::max()
				|| FT_New_Memory_Face(_library, static_cast<const FT_Byte*>(buffer.data()), static_cast<FT_Long>(buffer.size()), 0, &_face))
				throw DataError{ "Failed to load font" };
			_face_buffer = std::move(buffer);
		}
	};

	class FontImpl : public Font
	{
	public:
		FontImpl(const Source& source, RenderManager& render_manager, std::size_t size)
			: _size{ static_cast<int>(size) }
			, _image{ { size * 16, size * 8, PixelFormat::Gray8 } }
		{
			_freetype.load(source.to_buffer());
			_has_kerning = FT_HAS_KERNING(_freetype._face);
			FT_Set_Pixel_Sizes(_freetype._face, 0, static_cast<FT_UInt>(size));
			std::size_t x_offset = 0;
			std::size_t y_offset = 0;
			std::size_t row_height = 0;
			const auto baseline = static_cast<FT_Int>(size) * _freetype._face->ascender / _freetype._face->height;
			for (FT_UInt char_code = 0; char_code < 128; ++char_code)
			{
				const auto glyph_index = FT_Get_Char_Index(_freetype._face, char_code);
				if (!glyph_index)
					continue;

				if (FT_Load_Glyph(_freetype._face, glyph_index, FT_LOAD_RENDER))
					continue; // TODO: Report error.

				const auto glyph = _freetype._face->glyph;
				if (glyph->bitmap.width > _image.format().width() - x_offset)
				{
					x_offset = 0;
					y_offset += row_height + 1;
					row_height = 0;
				}
				if (glyph->bitmap.rows > _image.format().height() - y_offset)
					break; // TODO: Report error.
				auto src = glyph->bitmap.buffer;
				auto dst = static_cast<std::uint8_t*>(_image.data()) + _image.format().row_size() * y_offset + x_offset;
				for (unsigned y = 0; y < glyph->bitmap.rows; ++y)
				{
					std::memcpy(dst, src, glyph->bitmap.width);
					src += glyph->bitmap.width;
					dst += _image.format().row_size();
				}
				auto& font_char = _chars[static_cast<char>(char_code)];
				font_char.glyph_index = glyph_index;
				font_char.rect = { { static_cast<int>(x_offset), static_cast<int>(y_offset) }, Size{ static_cast<int>(glyph->bitmap.width), static_cast<int>(glyph->bitmap.rows) } };
				font_char.offset = { glyph->bitmap_left, baseline - glyph->bitmap_top };
				font_char.advance = static_cast<int>(glyph->advance.x >> 6);
				x_offset += glyph->bitmap.width + 1;
				if (row_height < glyph->bitmap.rows)
					row_height = glyph->bitmap.rows;
			}
			_texture = render_manager.create_texture_2d(_image, RenderManager::TextureFlag::Intensity);
		}

		void build(std::vector<TexturedRect>& rects, const Vector2& top_left, float font_size, std::string_view text, TextCapture* capture) const override
		{
			rects.clear();

			auto current_x = top_left.x;
			const auto current_y = top_left.y;
			const auto scaling = font_size / static_cast<float>(_size);

			float selection_left = 0;
			const auto do_capture = [font_size, capture, &current_x, current_y, &selection_left](std::size_t index) {
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

			auto previous = _chars.end();
			for (std::size_t i = 0; i < text.size(); ++i)
			{
				const auto current = _chars.find(text[i]);
				if (current == _chars.end())
					continue;
				if (_has_kerning && previous != _chars.end())
					current_x += static_cast<float>(kerning(previous->second.glyph_index, current->second.glyph_index)) * scaling;
				rects.emplace_back(
					RectF(
						{ current_x + static_cast<float>(current->second.offset._x) * scaling, current_y + static_cast<float>(current->second.offset._y) * scaling },
						SizeF(current->second.rect.size()) * scaling),
					RectF(current->second.rect));
				do_capture(i);
				current_x += static_cast<float>(current->second.advance) * scaling;
				previous = current;
			}

			do_capture(text.size());
		}

		Size text_size(std::string_view text) const override
		{
			int width = 0;
			auto previous = _chars.end();
			for (const auto c : text)
			{
				const auto current = _chars.find(c);
				if (current == _chars.end())
					continue;
				if (_has_kerning && previous != _chars.end())
					width += kerning(previous->second.glyph_index, current->second.glyph_index);
				width += current->second.advance;
				previous = current;
			}
			return { width, _size };
		}

		SizeF text_size(std::string_view text, const SizeF& font_size) const override
		{
			const SizeF size{ text_size(text) };
			return { font_size._width * (size._width * font_size._height / size._height), font_size._height };
		}

		const Texture2D* texture() const override
		{
			return _texture.get();
		}

	private:
		int kerning(FT_UInt left_glyph, FT_UInt right_glyph) const noexcept
		{
			FT_Vector kerning_vector;
			return !FT_Get_Kerning(_freetype._face, left_glyph, right_glyph, FT_KERNING_DEFAULT, &kerning_vector) ? static_cast<int>(kerning_vector.x >> 6) : 0;
		}

	private:
		struct FontChar
		{
			FT_UInt glyph_index = 0;
			Rect rect;
			Point offset;
			int advance = 0;
		};

		FreeTypeWrapper _freetype;
		const int _size;
		bool _has_kerning = false;
		Image _image;
		std::unordered_map<char, FontChar> _chars;
		std::shared_ptr<const Texture2D> _texture;
	};

	std::unique_ptr<Font> Font::load(const Source& source, RenderManager& render_manager)
	{
		return std::make_unique<FontImpl>(source, render_manager, 64);
	}
}
