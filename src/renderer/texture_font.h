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

	typedef std::map<char, TextureFont::CharInfo> Chars;

	typedef std::pair<char, char>   CharPair;
	typedef std::map<CharPair, int> Kernings;

public:

	int      _size;
	Chars    _chars;
	Kernings _kernings;
	Rect     _rect;
};

} // namespace Yttrium

#endif // __RENDERER_TEXTURE_FONT_H
