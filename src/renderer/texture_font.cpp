#include <yttrium/texture_font.h>

#include <yttrium/file.h>
#include <yttrium/utils.h>
#include "../base/private_base.h"
#include "texture_font.h"

#include <map>

namespace Yttrium
{
	class Y_PRIVATE TextureFont::Private: public PrivateBase<TextureFont::Private>
	{
	public:

		int _size = 0;
		std::map<char, TextureFont::CharInfo> _chars;
		std::map<std::pair<char, char>, int> _kernings;
		Rect _rect;

		Private(Allocator* allocator): PrivateBase(allocator) {}
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
		font->_rect.set_top_left(font_section.base_x, font_section.base_y);

		for (; char_count > 0; --char_count)
		{
			Ytf1Char char_data;
			if (!file.read(&char_data))
				return;

			CharInfo info;
			info.rect = Rect(font_section.base_x + char_data.x, font_section.base_y + char_data.y, char_data.width, char_data.height);
			info.offset = Point(char_data.x_offset, char_data.y_offset);
			info.advance = char_data.advance;

			font->_chars[char_data.id] = info;
			font->_rect.set_width(max(font->_rect.width(), char_data.x + char_data.width));
			font->_rect.set_height(max(font->_rect.height(), char_data.y + char_data.height));
		}

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
				font->_kernings[std::make_pair(kerning.first, kerning.second)] = kerning.amount;
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
		const auto i = _private->_kernings.find(std::make_pair(left, right));
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
		Size result(0, _private->_size);

		if (!text.is_empty())
		{
			for (size_t i = 0; i < text.size(); ++i)
			{
				const auto info = char_info(text[i]);
				if (info)
					result.width += info->advance;
				if (i > 0)
					result.width += kerning(text[i - 1], text[i]);
			}
		}

		return result;
	}

	Vector2 TextureFont::text_size(const StaticString& text, const Vector2& font_size) const
	{
		const Size& size = text_size(text);
		return Vector2(font_size.x * (size.width * font_size.y / size.height), font_size.y);
	}
}
