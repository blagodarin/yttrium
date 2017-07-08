#ifndef _src_renderer_debug_texture_h_
#define _src_renderer_debug_texture_h_

#include <cstdint>

namespace Yttrium
{
	namespace DebugTexture
	{
		constexpr uint8_t char_width = 8;
		constexpr uint8_t char_height = 16;

		constexpr uint8_t first_char = 33;
		constexpr uint8_t last_char = 126;

		constexpr uint8_t cursor_index = 0;
		constexpr uint8_t rect_index = 127;

		constexpr uint16_t width = 128;
		constexpr uint16_t height = 128;

		extern const int32_t data[];

		extern const float coords[][2][2];
	}
}

#endif
