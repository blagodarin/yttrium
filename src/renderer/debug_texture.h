#ifndef _src_renderer_debug_texture_h_
#define _src_renderer_debug_texture_h_

#include <cstdint>

namespace Yttrium
{
	namespace DebugTexture
	{
		extern const uint8_t char_width;
		extern const uint8_t char_height;

		extern const uint8_t first_char;
		extern const uint8_t last_char;

		extern const uint8_t cursor_index;
		extern const uint8_t rect_index;

		extern const uint16_t width;
		extern const uint16_t height;

		extern const int32_t data[];

		extern const float coords[][2][2];
	}
}

#endif
