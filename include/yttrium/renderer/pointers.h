/// \file
/// \brief

#ifndef __Y_RENDERER_POINTERS_H
#define __Y_RENDERER_POINTERS_H

#include <yttrium/object.h>

namespace Yttrium
{

class Texture2D;
class TextureCache;

/// 2D texture pointer.

typedef ObjectPointer<Texture2D> Texture2DPtr;

/// Texture cache pointer.

typedef ObjectPointer<TextureCache> TextureCachePtr;

} // namespace Yttrium

#endif // __Y_RENDERER_POINTERS_H
