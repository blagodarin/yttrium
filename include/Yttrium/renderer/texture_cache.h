/// \file
/// \brief

#ifndef __Y_RENDERER_TEXTURE_CACHE_H
#define __Y_RENDERER_TEXTURE_CACHE_H

#include <Yttrium/renderer/texture.h>
#include <Yttrium/static_string.h>

namespace Yttrium
{

class Y_API TextureCache
{
	friend class Renderer;

public:

	///

	TextureCache(const TextureCache &cache) noexcept;

	///

	~TextureCache() noexcept;

public:

	/// Clear the cache.

	void clear() noexcept;

	///

	Texture2D load_texture_2d(const StaticString &name) noexcept;

	///

	Texture2D cache_texture_2d(const StaticString &name) noexcept;

public:

	///

	inline operator bool() const noexcept;

	///

	TextureCache &operator =(const TextureCache &cache) noexcept;

public:

	class Private;

private:

	Y_PRIVATE TextureCache(Private *private_);

private:

	Private *_private;
};

////////////////////////////////////////////////////////////////////////////////

TextureCache::operator bool() const noexcept
{
	return _private;
}

} // namespace Yttrium

#endif // __Y_RENDERER_TEXTURE_CACHE_H
