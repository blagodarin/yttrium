#ifndef __RENDERER_TEXTURE_FONT_H
#define __RENDERER_TEXTURE_FONT_H

#include <yttrium/texture_font.h>

#include "../base/private_base.h"

#include <map>

namespace Yttrium
{

class Y_PRIVATE TextureFont::Private: public PrivateBase<TextureFont::Private>
{
public:

	Private(Allocator *allocator);

public:

	int                                   _size;
	std::map<char, TextureFont::CharInfo> _chars;
	std::map<std::pair<char, char>, int>  _kernings;
	Rect                                  _rect;
};

} // namespace Yttrium

#endif // __RENDERER_TEXTURE_FONT_H
