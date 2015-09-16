#ifndef __RENDERER_TEXTURE_FONT_H
#define __RENDERER_TEXTURE_FONT_H

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

	const uint32_t FourccYtf1 = Fourcc<'Y', 'T', 'F', '1'>::value;
	const uint32_t FourccFont = Fourcc<'f', 'o', 'n', 't'>::value;
	const uint32_t FourccChar = Fourcc<'c', 'h', 'a', 'r'>::value;
	const uint32_t FourccKern = Fourcc<'k', 'e', 'r', 'n'>::value;
}

#endif
