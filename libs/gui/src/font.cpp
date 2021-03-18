// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/font.h>

#include <yttrium/exceptions.h>
#include <yttrium/gui/text_capture.h>
#include <yttrium/image/image.h>
#include <yttrium/math/size.h>
#include <yttrium/renderer/2d.h>
#include <yttrium/renderer/manager.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>

#include <primal/utf8.hpp>

#include <cassert>
#include <cstring>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace
{
	constexpr size_t builtin_width = 4;
	constexpr size_t builtin_height = 4;
	constexpr uint8_t builtin_data[builtin_height][builtin_width]{
		{ 0xff, 0xff, 0x00, 0x00 },
		{ 0xff, 0xff, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00 },
	};
	constexpr Yt::RectF builtin_white_rect{ {}, Yt::SizeF{ 1, 1 } };
}

namespace Yt
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
			if (buffer.size() > static_cast<size_t>(std::numeric_limits<FT_Long>::max())
				|| FT_New_Memory_Face(_library, static_cast<const FT_Byte*>(buffer.data()), static_cast<FT_Long>(buffer.size()), 0, &_face))
				throw DataError{ "Failed to load font" };
			_face_buffer = std::move(buffer);
		}
	};

	class FontImpl : public Font
	{
	public:
		FontImpl(const Source& source, RenderManager& render_manager, size_t size)
			: _size{ static_cast<int>(size) }
			, _image{ { size * 32, size * 32, PixelFormat::Intensity8 } }
		{
			_freetype.load(source.to_buffer());
			_has_kerning = FT_HAS_KERNING(_freetype._face);
			FT_Set_Pixel_Sizes(_freetype._face, 0, static_cast<FT_UInt>(size));
			size_t x_offset = 0;
			size_t y_offset = 0;
			size_t row_height = 0;
			const auto copy_rect = [&](const uint8_t* src, size_t width, size_t height, ptrdiff_t stride) {
				if (height > 0)
				{
					if (stride < 0)
						src += (height - 1) * static_cast<size_t>(-stride);
					auto dst = static_cast<uint8_t*>(_image.data()) + _image.info().stride() * y_offset + x_offset;
					for (size_t y = 0; y < height; ++y)
					{
						std::memcpy(dst, src, width);
						src += stride;
						dst += _image.info().stride();
					}
					if (row_height < height)
						row_height = height;
				}
				x_offset += width + 1;
			};
			copy_rect(::builtin_data[0], ::builtin_width, ::builtin_height, ::builtin_width);
			const auto baseline = static_cast<FT_Int>(size) * _freetype._face->ascender / _freetype._face->height;
			for (FT_UInt char_code = 0; char_code < 65536; ++char_code)
			{
				const auto glyph_index = FT_Get_Char_Index(_freetype._face, char_code);
				if (!glyph_index)
					continue;

				if (FT_Load_Glyph(_freetype._face, glyph_index, FT_LOAD_RENDER))
					continue; // TODO: Report error.

				const auto glyph = _freetype._face->glyph;
				if (x_offset + glyph->bitmap.width > _image.info().width())
				{
					x_offset = 0;
					y_offset += row_height + 1;
					row_height = 0;
				}
				if (y_offset + glyph->bitmap.rows > _image.info().height())
					break; // TODO: Report error.
				auto& font_char = _chars[char_code];
				font_char.glyph_index = glyph_index;
				font_char.rect = { { static_cast<int>(x_offset), static_cast<int>(y_offset) }, Size{ static_cast<int>(glyph->bitmap.width), static_cast<int>(glyph->bitmap.rows) } };
				font_char.offset = { glyph->bitmap_left, baseline - glyph->bitmap_top };
				font_char.advance = static_cast<int>(glyph->advance.x >> 6);
				copy_rect(glyph->bitmap.buffer, glyph->bitmap.width, glyph->bitmap.rows, glyph->bitmap.pitch);
			}
			_texture = render_manager.create_texture_2d(_image);
		}

		void render(Renderer2D& renderer, const Vector2& topLeft, float fontSize, std::string_view text, TextCapture* capture) const override
		{
			auto x = topLeft.x;
			const auto y = topLeft.y;
			float selection_left = 0;
			const auto do_capture = [fontSize, capture, &x, y, &selection_left](size_t offset) {
				if (!capture)
					return;
				if (capture->_cursor_pos == offset)
				{
					capture->_cursor_rect = { { x, y }, SizeF{ 2, fontSize } };
					capture->_has_cursor = true;
				}
				if (capture->_selection_begin < capture->_selection_end)
				{
					if (offset == capture->_selection_begin)
					{
						selection_left = x;
					}
					else if (offset == capture->_selection_end)
					{
						capture->_selection_rect = { { selection_left, y }, Vector2{ x, y + fontSize } };
						capture->_has_selection = true;
					}
				}
			};

			renderer.setTexture(_texture);
			const auto scaling = fontSize / static_cast<float>(_size);
			auto previous = _chars.end();
			for (size_t i = 0; i < text.size();)
			{
				const auto offset = i;
				const auto current = _chars.find(primal::readUtf8(text, i));
				if (current == _chars.end())
					continue;
				if (_has_kerning && previous != _chars.end())
				{
					FT_Vector kerning;
					if (!FT_Get_Kerning(_freetype._face, previous->second.glyph_index, current->second.glyph_index, FT_KERNING_DEFAULT, &kerning))
						x += static_cast<float>(kerning.x >> 6) * scaling;
				}
				renderer.setTextureRect(Yt::RectF{ current->second.rect });
				renderer.addRect({ { x + static_cast<float>(current->second.offset._x) * scaling, y + static_cast<float>(current->second.offset._y) * scaling },
					SizeF(current->second.rect.size()) * scaling });
				do_capture(offset);
				x += static_cast<float>(current->second.advance) * scaling;
				previous = current;
			}
			do_capture(text.size());
		}

		Size text_size(std::string_view text) const override
		{
			int width = 0;
			auto previous = _chars.end();
			for (size_t i = 0; i < text.size();)
			{
				const auto current = _chars.find(primal::readUtf8(text, i));
				if (current == _chars.end())
					continue;
				if (_has_kerning && previous != _chars.end())
				{
					FT_Vector kerning;
					if (!FT_Get_Kerning(_freetype._face, previous->second.glyph_index, current->second.glyph_index, FT_KERNING_DEFAULT, &kerning))
						width += static_cast<int>(kerning.x >> 6);
				}
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

		std::shared_ptr<const Texture2D> texture() const noexcept override
		{
			return _texture;
		}

		RectF white_rect() const noexcept override
		{
			return ::builtin_white_rect;
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
		std::unordered_map<char32_t, FontChar> _chars;
		std::shared_ptr<const Texture2D> _texture;
	};

	std::shared_ptr<const Font> Font::load(const Source& source, RenderManager& render_manager)
	{
		return std::make_shared<FontImpl>(source, render_manager, 64);
	}
}
