/// \file
/// \brief Texture cache.

#ifndef __Y_RENDERER_TEXTURE_CACHE_H
#define __Y_RENDERER_TEXTURE_CACHE_H

#include <yttrium/renderer/pointers.h>

namespace Yttrium
{

class Renderer;
class StaticString;

/// Texture cache.

class Y_API TextureCache: public Object
{
public:

	///

	virtual Texture2DPtr cache_texture_2d(const StaticString &name, bool intensity = false) noexcept = 0;

	/// Clear the cache.

	virtual void clear() noexcept = 0;

	///

	virtual Texture2DPtr load_texture_2d(const StaticString &name, bool intensity = false) noexcept = 0;

public:

	///

	static TextureCachePtr create(const Renderer &renderer) noexcept;

protected:

	TextureCache(Allocator *allocator) noexcept: Object(allocator) {}
};

} // namespace Yttrium

#endif // __Y_RENDERER_TEXTURE_CACHE_H
