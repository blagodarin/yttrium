#include <yttrium/renderer/texture_font.h>

#include <yttrium/io/errors.h>
#include <yttrium/io/reader.h>
#include <yttrium/log.h>
#include <yttrium/renderer/text_capture.h>
#include <yttrium/renderer/textured_rect.h>
#include <yttrium/std/map.h>
#include <yttrium/utils.h>
#include "texture_font.h"

namespace Yttrium
{
	class TextureFontImpl : public TextureFont
	{
	public:
		TextureFontImpl(Allocator& allocator, int size)
			: _size(size)
			, _chars(allocator)
			, _kernings(allocator)
		{
		}

		void build(StdVector<TexturedRect>& rects, const PointF& top_left, float font_size, const StaticString& text, TextCapture* capture) const override
		{
			rects.clear();

			float current_x = top_left.x();
			float current_y = top_left.y();
			char last_symbol = '\0';
			const float scaling = font_size / _size;

			float selection_left = 0;
			const auto do_capture = [&](unsigned index)
			{
				if (!capture)
					return;

				if (capture->cursor_pos == index)
				{
					capture->cursor_rect = {{current_x, current_y + font_size * 0.125f}, SizeF(2, font_size * 0.75f)};
					capture->has_cursor = true;
				}

				if (capture->selection_begin < capture->selection_end)
				{
					if (index == capture->selection_begin)
					{
						selection_left = current_x;
					}
					else if (index == capture->selection_end)
					{
						capture->selection_rect = {{selection_left, current_y}, PointF(current_x, current_y + font_size)};
						capture->has_selection = true;
					}
				}
			};

			const char* current_symbol = text.text();

			for (size_t i = 0; i < text.size(); ++i, ++current_symbol)
			{
				const auto info = _chars.find(*current_symbol);
				if (info != _chars.end())
				{
					rects.emplace_back(
						RectF(
							{ current_x + info->second.offset.x() * scaling, current_y + info->second.offset.y() * scaling },
							SizeF(info->second.rect.size()) * scaling
						),
						RectF(info->second.rect)
					);
					do_capture(i);
					const auto kerning = _kernings.find({last_symbol, *current_symbol});
					current_x += (info->second.advance + (kerning != _kernings.end() ? kerning->second : 0)) * scaling;
				}
				last_symbol = *current_symbol;
			}

			do_capture(text.size());
		}

		Rect rect() const override
		{
			return _rect;
		}

		Size text_size(const StaticString& text) const override
		{
			int width = 0;
			for (size_t i = 0; i < text.size(); ++i)
			{
				const auto info = _chars.find(text[i]);
				if (info != _chars.end())
					width += info->second.advance;
				if (i > 0)
				{
					const auto kerning = _kernings.find({text[i - 1], text[i]});
					if (kerning != _kernings.end())
						width += kerning->second;
				}
			}
			return {width, _size};
		}

		SizeF text_size(const StaticString& text, const SizeF& font_size) const override
		{
			const auto& size = text_size(text);
			return {font_size.width() * (size.width() * font_size.height() / size.height()), font_size.height()};
		}

	private:
		const int _size;
		StdMap<char, TextureFont::CharInfo> _chars; // TODO: Flat map.
		StdMap<std::pair<char, char>, int> _kernings;
		Rect _rect;

		friend TextureFont;
	};

	ResourcePtr<TextureFont> TextureFont::load(Reader&& reader, Allocator& allocator)
	{
		if (!reader)
			throw ResourceError(allocator) << "Bad file"_s;

		reader.seek(0);

		uint32_t fourcc = 0;
		if (!reader.read(fourcc) || fourcc != FourccYtf1)
			throw ResourceError(allocator) << "Bad 'YTF1' fourcc"_s;

		if (!reader.read(fourcc) || fourcc != FourccFont)
			throw ResourceError(allocator) << "Bad 'font' section fourcc"_s;

		Ytf1Font font_section;
		if (!reader.read(font_section))
			throw ResourceError(allocator) << "Bad 'font' section"_s;

		if (!reader.read(fourcc) || fourcc != FourccChar)
			throw ResourceError(allocator) << "Bad 'char' section fourcc"_s;

		uint8_t char_count = 0;
		if (!reader.read(char_count))
			throw ResourceError(allocator) << "Bad 'char' section header"_s;

		auto font = make_resource<TextureFontImpl>(allocator, font_section.size);

		Size font_rect_size;
		for (uint8_t i = 0; i < char_count; ++i)
		{
			Ytf1Char char_data;
			if (!reader.read(char_data))
				throw ResourceError(allocator) << "Bad 'char' section entry "_s << i;

			CharInfo info;
			info.rect = {{font_section.base_x + char_data.x, font_section.base_y + char_data.y}, Size(char_data.width, char_data.height)};
			info.offset = Point(char_data.x_offset, char_data.y_offset);
			info.advance = char_data.advance;

			font->_chars[char_data.id] = info;
			font_rect_size = max(font_rect_size, {char_data.x + char_data.width, char_data.y + char_data.height});
		}
		font->_rect = {{font_section.base_x, font_section.base_y}, font_rect_size};

		if (reader.read(fourcc))
		{
			if (fourcc != FourccKern)
				throw ResourceError(allocator) << "Bad 'kern' section fourcc"_s;

			uint16_t kerning_count = 0;
			if (!reader.read(kerning_count))
				throw ResourceError(allocator) << "Bad 'kern' section header"_s;

			for (uint16_t i = 0; i < kerning_count; ++i)
			{
				Ytf1Kerning kerning;
				if (!reader.read(kerning))
					throw ResourceError(allocator) << "Bad 'kern' section entry "_s << i;
				font->_kernings[{kerning.first, kerning.second}] = kerning.amount;
			}
		}

		return std::move(font);
	}
}
