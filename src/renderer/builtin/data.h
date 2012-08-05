#ifndef __RENDERER_BUILTIN_DATA_H
#define __RENDERER_BUILTIN_DATA_H

#include <Yttrium/types.h>

namespace Yttrium
{

namespace Builtin
{

extern const uint8_t char_width;
extern const uint8_t char_height;

extern const uint8_t first_char;
extern const uint8_t last_char;

extern const uint8_t font_cursor;

extern const uint16_t texture_width;
extern const uint16_t texture_height;

extern const uint16_t h_chars;
extern const uint16_t v_chars;

extern const int32_t texture[];

extern const float coords[][2][2];

} // namespace Builtin

} // namespace Yttrium

#endif // __RENDERER_BUILTIN_DATA_H
