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
#include <yttrium/renderer/manager.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include "font_data.h"
#include "texture_font.h"

#include <cassert>
#include <cstring>

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

	class FontPrivate
	{
	public:
		void load(const Source& source, const std::shared_ptr<const Texture2D>& texture)
		{
			Reader reader{ source };

			std::uint32_t fourcc = 0;
			if (!reader.read(fourcc) || fourcc != FourccYtf1)
				throw DataError("Bad 'YTF1' fourcc");

			if (!reader.read(fourcc) || fourcc != FourccFont)
				throw DataError("Bad 'font' section fourcc");

			Ytf1Font font_section;
			if (!reader.read(font_section))
				throw DataError("Bad 'font' section");

			if (!reader.read(fourcc) || fourcc != FourccChar)
				throw DataError("Bad 'char' section fourcc");

			std::uint8_t char_count = 0;
			if (!reader.read(char_count))
				throw DataError("Bad 'char' section header");

			FontData data;
			data._size = font_section.size;

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

				_data._chars[char_data.id] = info;
				font_rect_size = { std::max(font_rect_size._width, char_data.x + char_data.width), std::max(font_rect_size._height, char_data.y + char_data.height) };
			}

			if (!Rect(texture->size()).contains({ { font_section.base_x, font_section.base_y }, font_rect_size }))
				throw DataError{ "Font texture size mismatch" };

			if (reader.read(fourcc))
			{
				if (fourcc != FourccKern)
					throw DataError("Bad 'kern' section fourcc");

				std::uint16_t kerning_count = 0;
				if (!reader.read(kerning_count))
					throw DataError("Bad 'kern' section header");

				for (std::uint16_t i = 0; i < kerning_count; ++i)
				{
					Ytf1Kerning kerning;
					if (!reader.read(kerning))
						throw DataError("Bad 'kern' section entry ", i);
					_data._kernings.emplace(FontData::key(kerning.first, kerning.second), kerning.amount);
				}
			}

			_texture = texture;
			_data = std::move(data);
		}

		void render(RenderManager& render_manager, std::size_t size)
		{
			assert(_freetype);
			assert(_freetype->_face);
			assert(!_texture);
			const ImageFormat image_format{ size * 16, size * 8, PixelFormat::Gray8 };
			Image image{ image_format };
			std::unordered_map<char, FontChar> chars;
			FT_Set_Pixel_Sizes(_freetype->_face, 0, static_cast<FT_UInt>(_data._size));
			std::size_t x_offset = 0;
			std::size_t y_offset = 0;
			for (FT_UInt char_code = 0; char_code < 128; ++char_code)
			{
				if (FT_Load_Char(_freetype->_face, char_code, FT_LOAD_RENDER))
					continue; // TODO: Report error.
				if (_freetype->_face->glyph->bitmap.width > image_format.width() - x_offset)
				{
					x_offset = 0;
					y_offset += size;
				}
				if (_freetype->_face->glyph->bitmap.rows > image_format.height() - y_offset)
					break; // TODO: Report error.
				auto src = _freetype->_face->glyph->bitmap.buffer;
				auto dst = static_cast<std::uint8_t*>(image.data()) + image_format.row_size() * y_offset + x_offset;
				for (unsigned y = 0; y < _freetype->_face->glyph->bitmap.rows; ++y)
				{
					std::memcpy(dst, src, _freetype->_face->glyph->bitmap.width);
					src += _freetype->_face->glyph->bitmap.width;
					dst += image_format.row_size();
				}
				auto font_char = chars[static_cast<char>(char_code)];
				font_char.rect = { { static_cast<int>(x_offset), static_cast<int>(y_offset) }, Size{ static_cast<int>(_freetype->_face->glyph->bitmap.width), static_cast<int>(_freetype->_face->glyph->bitmap.rows) } };
				font_char.offset = { 0, 0 };
				font_char.advance = font_char.rect.width();
			}
			_texture = render_manager.create_texture_2d(std::move(image), RenderManager::TextureFlag::Intensity);
			_data._size = static_cast<int>(size);
			_data._chars = std::move(chars);
			_data._kernings.clear();
		}

	private:
		std::optional<FreeTypeWrapper> _freetype;
		FontData _data;
		std::shared_ptr<const Texture2D> _texture;

		friend Font;
	};

	class FontImpl : public Font
	{
	public:
		FontImpl(const Source& source, RenderManager& render_manager, std::size_t size)
		{
			_freetype.emplace();
			_freetype->load(source.to_buffer());
			const ImageFormat image_format{ size * 16, size * 8, PixelFormat::Gray8 };
			Image image{ image_format };
			std::unordered_map<char, FontChar> chars;
			FT_Set_Pixel_Sizes(_freetype->_face, 0, static_cast<FT_UInt>(_data._size));
			std::size_t x_offset = 0;
			std::size_t y_offset = 0;
			for (FT_UInt char_code = 0; char_code < 128; ++char_code)
			{
				if (FT_Load_Char(_freetype->_face, char_code, FT_LOAD_RENDER))
					continue; // TODO: Report error.
				if (_freetype->_face->glyph->bitmap.width > image_format.width() - x_offset)
				{
					x_offset = 0;
					y_offset += size;
				}
				if (_freetype->_face->glyph->bitmap.rows > image_format.height() - y_offset)
					break; // TODO: Report error.
				auto src = _freetype->_face->glyph->bitmap.buffer;
				auto dst = static_cast<std::uint8_t*>(image.data()) + image_format.row_size() * y_offset + x_offset;
				for (unsigned y = 0; y < _freetype->_face->glyph->bitmap.rows; ++y)
				{
					std::memcpy(dst, src, _freetype->_face->glyph->bitmap.width);
					src += _freetype->_face->glyph->bitmap.width;
					dst += image_format.row_size();
				}
				auto font_char = chars[static_cast<char>(char_code)];
				font_char.rect = { { static_cast<int>(x_offset), static_cast<int>(y_offset) }, Size{ static_cast<int>(_freetype->_face->glyph->bitmap.width), static_cast<int>(_freetype->_face->glyph->bitmap.rows) } };
				font_char.offset = { 0, 0 };
				font_char.advance = font_char.rect.width();
			}
			_texture = render_manager.create_texture_2d(std::move(image), RenderManager::TextureFlag::Intensity);
			_data._size = static_cast<int>(size);
			_data._chars = std::move(chars);
			_data._kernings.clear();
		}

		FontImpl(const Source& source, const std::shared_ptr<const Texture2D>& texture)
			: _texture{ texture }
		{
			Reader reader{ source };

			std::uint32_t fourcc = 0;
			if (!reader.read(fourcc) || fourcc != FourccYtf1)
				throw DataError("Bad 'YTF1' fourcc");

			if (!reader.read(fourcc) || fourcc != FourccFont)
				throw DataError("Bad 'font' section fourcc");

			Ytf1Font font_section;
			if (!reader.read(font_section))
				throw DataError("Bad 'font' section");

			if (!reader.read(fourcc) || fourcc != FourccChar)
				throw DataError("Bad 'char' section fourcc");

			std::uint8_t char_count = 0;
			if (!reader.read(char_count))
				throw DataError("Bad 'char' section header");

			_data._size = font_section.size;

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

				_data._chars[char_data.id] = info;
				font_rect_size = { std::max(font_rect_size._width, char_data.x + char_data.width), std::max(font_rect_size._height, char_data.y + char_data.height) };
			}

			if (!Rect(texture->size()).contains({ { font_section.base_x, font_section.base_y }, font_rect_size }))
				throw DataError{ "Font texture size mismatch" };

			if (reader.read(fourcc))
			{
				if (fourcc != FourccKern)
					throw DataError("Bad 'kern' section fourcc");

				std::uint16_t kerning_count = 0;
				if (!reader.read(kerning_count))
					throw DataError("Bad 'kern' section header");

				for (std::uint16_t i = 0; i < kerning_count; ++i)
				{
					Ytf1Kerning kerning;
					if (!reader.read(kerning))
						throw DataError("Bad 'kern' section entry ", i);
					_data._kernings.emplace(FontData::key(kerning.first, kerning.second), kerning.amount);
				}
			}
		}

		void build(std::vector<TexturedRect>& rects, const Vector2& top_left, float font_size, std::string_view text, TextCapture* capture) const override
		{
			_data.build(rects, top_left, font_size, text, capture);
		}

		Size text_size(std::string_view text) const override
		{
			return _data.text_size(text);
		}

		SizeF text_size(std::string_view text, const SizeF& font_size) const override
		{
			return _data.text_size(text, font_size);
		}

		const Texture2D* texture() const override
		{
			return _texture.get();
		}

	private:
		FontData _data;
		std::shared_ptr<const Texture2D> _texture;
		std::optional<FreeTypeWrapper> _freetype;
	};

	std::unique_ptr<Font> Font::load(const Source& source, RenderManager& render_manager)
	{
		return std::make_unique<FontImpl>(source, render_manager, 32);
	}

	std::unique_ptr<Font> Font::load(const Source& source, const std::shared_ptr<const Texture2D>& texture)
	{
		return std::make_unique<FontImpl>(source, texture);
	}
}
