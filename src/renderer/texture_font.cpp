#include <yttrium/texture_font.h>

#include <yttrium/file.h>
#include <yttrium/std/map.h>
#include <yttrium/utils.h>
#include "../base/private_base.h"
#include "texture_font.h"

namespace Yttrium
{
	class Y_PRIVATE TextureFont::Private: public PrivateBase<TextureFont::Private>
	{
	public:

		int _size = 0;
		StdMap<char, TextureFont::CharInfo> _chars;
		StdMap<std::pair<char, char>, int> _kernings;
		Rect _rect;

		Private(Allocator* allocator)
			: PrivateBase(allocator)
			, _chars(*allocator)
			, _kernings(*allocator)
		{
		}
	};

	Y_IMPLEMENT_SHARED(TextureFont);

	TextureFont::TextureFont(const StaticString& name, Allocator* allocator)
	{
		File file(name, allocator);
		if (!file)
			return;

		uint32_t fourcc = 0;
		if (!file.read(&fourcc) || fourcc != FourccYtf1)
			return;

		if (!file.read(&fourcc) || fourcc != FourccFont)
			return;

		Ytf1Font font_section;
		if (!file.read(&font_section))
			return;

		if (!file.read(&fourcc) || fourcc != FourccChar)
			return;

		uint8_t char_count = 0;
		if (!file.read(&char_count))
			return;

		PrivateHolder<TextureFont::Private> font(allocator);
		font->_size = font_section.size;

		Size font_rect_size;
		for (; char_count > 0; --char_count)
		{
			Ytf1Char char_data;
			if (!file.read(&char_data))
				return;

			CharInfo info;
			info.rect = {{font_section.base_x + char_data.x, font_section.base_y + char_data.y}, Size(char_data.width, char_data.height)};
			info.offset = Point(char_data.x_offset, char_data.y_offset);
			info.advance = char_data.advance;

			font->_chars[char_data.id] = info;
			font_rect_size = max(font_rect_size, {char_data.x + char_data.width, char_data.y + char_data.height});
		}
		font->_rect = {{font_section.base_x, font_section.base_y}, font_rect_size};

		if (file.read(&fourcc))
		{
			if (fourcc != FourccKern)
				return;

			uint16_t kerning_count = 0;
			if (!file.read(&kerning_count))
				return;

			for (; kerning_count > 0; --kerning_count)
			{
				Ytf1Kerning kerning;
				if (!file.read(&kerning))
					return;
				font->_kernings[{kerning.first, kerning.second}] = kerning.amount;
			}
		}

		_private = font.release();
	}

	const TextureFont::CharInfo* TextureFont::char_info(char symbol) const
	{
		const auto i = _private->_chars.find(symbol);
		return i == _private->_chars.end() ? nullptr : &i->second;
	}

	int TextureFont::kerning(char left, char right) const
	{
		const auto i = _private->_kernings.find({left, right});
		return i != _private->_kernings.end() ? i->second : 0;
	}

	Rect TextureFont::rect() const
	{
		return _private->_rect;
	}

	int TextureFont::size() const
	{
		return _private->_size;
	}

	Size TextureFont::text_size(const StaticString& text) const
	{
		int width = 0;
		if (!text.is_empty())
		{
			for (size_t i = 0; i < text.size(); ++i)
			{
				const auto info = char_info(text[i]);
				if (info)
					width += info->advance;
				if (i > 0)
					width += kerning(text[i - 1], text[i]);
			}
		}
		return {width, _private->_size};
	}

	SizeF TextureFont::text_size(const StaticString& text, const SizeF& font_size) const
	{
		const auto& size = text_size(text);
		return {font_size.width() * (size.width() * font_size.height() / size.height()), font_size.height()};
	}
}
