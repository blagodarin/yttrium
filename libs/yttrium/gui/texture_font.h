#ifndef _src_gui_texture_font_h_
#define _src_gui_texture_font_h_

#include "../utils/fourcc.h"

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
		char     id;
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
		char   first;
		char   second;
		int8_t amount;
	};

#pragma pack(pop)

	constexpr auto FourccYtf1 = "YTF1"_fourcc;
	constexpr auto FourccFont = "font"_fourcc;
	constexpr auto FourccChar = "char"_fourcc;
	constexpr auto FourccKern = "kern"_fourcc;
}

#endif
