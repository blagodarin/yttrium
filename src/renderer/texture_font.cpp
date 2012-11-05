#include "texture_font.h"

#include <Yttrium/file.h>
#include <Yttrium/utils.h>

#include "../base/fourcc.h"

namespace Yttrium
{

// Yttrium texture font format version 1 (YTF-1).

#pragma pack(push, 1)

struct Ytf1Font
{
	uint8_t  size;
	uint16_t base_x;
	uint16_t base_y;
};

struct Ytf1Char
{
	uint8_t  id;
	uint16_t x;
	uint16_t y;
	uint8_t  width;
	uint8_t  height;
	int8_t   x_offset;
	int8_t   y_offset;
	uint8_t  advance;
};

struct Ytf1Kerning
{
	uint8_t first;
	uint8_t second;
	int8_t  amount;
};

#pragma pack(pop)

namespace
{

const uint32_t FourccYtf1 = Fourcc<'Y', 'T', 'F', '1'>::value;
const uint32_t FourccFont = Fourcc<'f', 'o', 'n', 't'>::value;
const uint32_t FourccChar = Fourcc<'c', 'h', 'a', 'r'>::value;
const uint32_t FourccKern = Fourcc<'k', 'e', 'r', 'n'>::value;

// Font FOURCCs from the past generations required by the present tests and examples.
// TODO: Remove.

const uint32_t FourccEtf1 = Fourcc<'E', 'T', 'F', '1'>::value;
const uint32_t FourccGvf1 = Fourcc<'G', 'V', 'F', '1'>::value;

} // namespace

TextureFont::Private::Private(Allocator *allocator)
	: PrivateBase(allocator)
{
}

TextureFont::TextureFont(const TextureFont &font)
	: _private(Private::copy(font._private))
{
}

Area TextureFont::area() const
{
	return _private->_area;
}

const TextureFont::CharInfo *TextureFont::char_info(char symbol) const
{
	Private::Chars::const_iterator i = _private->_chars.find(symbol);
	return i == _private->_chars.end() ? nullptr : &i->second;
}

void TextureFont::close()
{
	Private::release(&_private);
}

Dim TextureFont::kerning(char left, char right) const
{
	Private::Kernings::const_iterator i = _private->_kernings.find(Private::CharPair(left, right));
	return i != _private->_kernings.end() ? i->second : 0;
}

bool TextureFont::open(const StaticString &name, Allocator *allocator)
{
	close();

	File file(name, allocator);

	if (!file.is_opened())
	{
		return false;
	}

	uint32_t fourcc;

	if (!(file.read(&fourcc) && (fourcc == FourccYtf1 || fourcc == FourccGvf1 || fourcc == FourccEtf1)))
	{
		return false;
	}

	Ytf1Font font_section;

	if (!(file.read(&fourcc) && fourcc == FourccFont && file.read(&font_section)))
	{
		return false;
	}

	_private = new(allocator->allocate<Private>()) Private(allocator);

	_private->_size = font_section.size;
	_private->_area.set_top_left(font_section.base_x, font_section.base_y);

	uint8_t char_count;

	if (file.read(&fourcc) && fourcc == FourccChar && file.read(&char_count))
	{
		bool success = true;

		for (; char_count; --char_count)
		{
			Ytf1Char char_data;

			if (!file.read(&char_data))
			{
				success = false;
				break;
			}

			CharInfo info;

			info.area = Area(font_section.base_x + char_data.x, font_section.base_y + char_data.y, char_data.width, char_data.height);
			info.offset = Dim2(char_data.x_offset, char_data.y_offset);
			info.advance = char_data.advance;

			_private->_chars[char_data.id] = info;
			_private->_area.set_width(max(_private->_area.width(), char_data.x + char_data.width));
			_private->_area.set_height(max(_private->_area.height(), char_data.y + char_data.height));
		}

		if (success && file.read(&fourcc))
		{
			uint16_t kerning_count;

			success = (fourcc == FourccKern) && file.read(&kerning_count);

			if (success)
			{
				for (; kerning_count; --kerning_count)
				{
					Ytf1Kerning kerning;

					if (!file.read(&kerning))
					{
						success = false;
						break;
					}

					_private->_kernings[Private::CharPair(kerning.first, kerning.second)] = kerning.amount;
				}
			}
		}

		if (success)
		{
			return true;
		}
	}

	close();

	return false;
}

Dim TextureFont::size() const
{
	return _private->_size;
}

Dim2 TextureFont::text_size(const StaticString &text) const
{
	Dim2 result(0, _private->_size);

	if (!text.is_empty())
	{
		// TODO: Rewrite.

		char previous = text[0];
		const CharInfo *info = char_info(previous);
		if (info)
		{
			result.x += info->advance;
		}

		for (size_t i = 1; i < text.size(); ++i)
		{
			char current = text[i];
			const CharInfo *info = char_info(current);
			if (info)
			{
				result.x += info->advance + kerning(previous, current);
			}
			previous = current;
		}
	}

	return result;
}

TextureFont &TextureFont::operator =(const TextureFont &font)
{
	Private::assign(&_private, font._private);

	return *this;
}

} // namespace Yttrium
