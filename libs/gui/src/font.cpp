// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/font.h>

#include <yttrium/base/exceptions.h>
#include <yttrium/renderer/2d.h>
#include <yttrium/renderer/manager.h>
#include <yttrium/renderer/texture.h>

#include <seir_base/utf8.hpp>
#include <seir_data/blob.hpp>
#include <seir_graphics/rectf.hpp>
#include <seir_image/image.hpp>

#include <cassert>
#include <cstring>
#include <optional>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace
{
	constexpr size_t builtinWidth = 4;
	constexpr size_t builtinHeight = 4;
	constexpr uint8_t builtinData[builtinHeight][builtinWidth]{
		{ 0xff, 0xff, 0x00, 0x00 },
		{ 0xff, 0xff, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00 },
	};
	constexpr seir::RectF builtinWhiteRect{ {}, seir::SizeF{ 1, 1 } };
}

namespace Yt
{
	struct FreeTypeWrapper
	{
		FT_Library _library = nullptr;
		seir::SharedPtr<seir::Blob> _faceBlob;
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

		void load(const seir::SharedPtr<seir::Blob>& blob)
		{
			assert(!_face);
			assert(blob);
			if (blob->size() > static_cast<size_t>(std::numeric_limits<FT_Long>::max())
				|| FT_New_Memory_Face(_library, static_cast<const FT_Byte*>(blob->data()), static_cast<FT_Long>(blob->size()), 0, &_face))
				throw DataError{ "Failed to load font" };
			_faceBlob = blob;
		}
	};

	class FontImpl final : public Font
	{
	public:
		FontImpl(const seir::SharedPtr<seir::Blob>& blob, RenderManager& renderManager, uint32_t size)
			: _size{ static_cast<int>(size) }
		{
			_freetype.load(blob);
			_hasKerning = FT_HAS_KERNING(_freetype._face);
			FT_Set_Pixel_Sizes(_freetype._face, 0, size);
			const seir::ImageInfo imageInfo{ size * 32, size * 32, seir::PixelFormat::Intensity8 };
			seir::Buffer buffer{ imageInfo.frameSize() };
			size_t x_offset = 0;
			size_t y_offset = 0;
			size_t row_height = 0;
			const auto copy_rect = [&](const uint8_t* src, size_t width, size_t height, ptrdiff_t stride) {
				if (height > 0)
				{
					if (stride < 0)
						src += (height - 1) * static_cast<size_t>(-stride);
					auto dst = buffer.data() + imageInfo.stride() * y_offset + x_offset;
					for (size_t y = 0; y < height; ++y)
					{
						std::memcpy(dst, src, width);
						src += stride;
						dst += imageInfo.stride();
					}
					if (row_height < height)
						row_height = height;
				}
				x_offset += width + 1;
			};
			copy_rect(::builtinData[0], ::builtinWidth, ::builtinHeight, ::builtinWidth);
			const auto baseline = static_cast<FT_Int>(size) * _freetype._face->ascender / _freetype._face->height;
			for (FT_UInt codepoint = 0; codepoint < 65536; ++codepoint)
			{
				const auto id = FT_Get_Char_Index(_freetype._face, codepoint);
				if (!id)
					continue;
				if (FT_Load_Glyph(_freetype._face, id, FT_LOAD_RENDER))
					continue; // TODO: Report error.
				const auto glyph = _freetype._face->glyph;
				if (x_offset + glyph->bitmap.width > imageInfo.width())
				{
					x_offset = 0;
					y_offset += row_height + 1;
					row_height = 0;
				}
				if (y_offset + glyph->bitmap.rows > imageInfo.height())
					break; // TODO: Report error.
				auto& glyphInfo = _glyph[codepoint];
				glyphInfo._id = id;
				glyphInfo._rect = { { static_cast<int>(x_offset), static_cast<int>(y_offset) }, seir::Size{ static_cast<int>(glyph->bitmap.width), static_cast<int>(glyph->bitmap.rows) } };
				glyphInfo._offset = { glyph->bitmap_left, baseline - glyph->bitmap_top };
				glyphInfo._advance = static_cast<int>(glyph->advance.x >> 6);
				copy_rect(glyph->bitmap.buffer, glyph->bitmap.width, glyph->bitmap.rows, glyph->bitmap.pitch);
			}
			_texture = renderManager.create_texture_2d({ imageInfo, std::move(buffer) });
		}

		void render(Renderer2D& renderer, const seir::RectF& rect, std::string_view text) const override
		{
			const auto scale = rect.height() / static_cast<float>(_size);
			int x = 0;
			auto previous = _glyph.end();
			renderer.setTexture(_texture);
			for (size_t i = 0; i < text.size();)
			{
				const auto current = _glyph.find(seir::readUtf8(text, i));
				if (current == _glyph.end())
					continue;
				if (_hasKerning && previous != _glyph.end())
				{
					FT_Vector kerning;
					if (!FT_Get_Kerning(_freetype._face, previous->second._id, current->second._id, FT_KERNING_DEFAULT, &kerning))
						x += static_cast<int>(kerning.x >> 6);
				}
				const auto left = rect.left() + static_cast<float>(x + current->second._offset._x) * scale;
				if (left >= rect.right())
					break;
				seir::RectF positionRect{ { left, rect.top() + static_cast<float>(current->second._offset._y) * scale }, seir::SizeF{ current->second._rect.size() } * scale };
				seir::RectF glyphRect{ current->second._rect };
				bool clipped = false;
				if (positionRect.right() > rect.right())
				{
					const auto originalWidth = positionRect.width();
					positionRect._right = rect._right;
					glyphRect.setWidth(glyphRect.width() * positionRect.width() / originalWidth);
					clipped = true;
				}
				renderer.setTextureRect(glyphRect);
				renderer.addBorderlessRect(positionRect);
				if (clipped)
					return;
				x += current->second._advance;
				previous = current;
			}
		}

		float textWidth(std::string_view text, float fontSize, TextCapture* capture) const override
		{
			const auto scale = fontSize / static_cast<float>(_size);

			int x = 0;
			std::optional<float> selectionX;
			const auto updateCapture = [&](size_t offset) {
				if (!capture)
					return;
				if (capture->_cursorOffset == offset)
					capture->_cursorPosition.emplace(static_cast<float>(x) * scale);
				if (capture->_selectionBegin < capture->_selectionEnd)
				{
					if (selectionX)
					{
						if (offset == capture->_selectionEnd)
						{
							capture->_selectionRange.emplace(*selectionX, static_cast<float>(x) * scale);
							selectionX.reset();
						}
					}
					else if (offset == capture->_selectionBegin)
						selectionX = static_cast<float>(x) * scale;
				}
			};

			auto previous = _glyph.end();
			for (size_t i = 0; i < text.size();)
			{
				const auto offset = i;
				const auto current = _glyph.find(seir::readUtf8(text, i));
				if (current == _glyph.end())
					continue;
				if (_hasKerning && previous != _glyph.end())
				{
					FT_Vector kerning;
					if (!FT_Get_Kerning(_freetype._face, previous->second._id, current->second._id, FT_KERNING_DEFAULT, &kerning))
						x += static_cast<int>(kerning.x >> 6);
				}
				updateCapture(offset);
				x += current->second._advance;
				previous = current;
			}
			updateCapture(text.size());
			return static_cast<float>(x) * scale;
		}

		std::shared_ptr<const Texture2D> texture() const noexcept override
		{
			return _texture;
		}

		seir::RectF textureRect(Graphics graphics) const noexcept override
		{
			switch (graphics)
			{
			case Graphics::WhiteRect: return ::builtinWhiteRect;
			}
			return {};
		}

	private:
		struct Glyph
		{
			FT_UInt _id = 0;
			seir::Rect _rect;
			seir::Point _offset;
			int _advance = 0;
		};

		FreeTypeWrapper _freetype;
		const int _size;
		bool _hasKerning = false;
		std::unordered_map<char32_t, Glyph> _glyph;
		std::shared_ptr<const Texture2D> _texture;
	};

	std::shared_ptr<const Font> Font::load(const seir::SharedPtr<seir::Blob>& blob, RenderManager& renderManager)
	{
		return blob ? std::make_shared<FontImpl>(blob, renderManager, 64) : nullptr;
	}
}
