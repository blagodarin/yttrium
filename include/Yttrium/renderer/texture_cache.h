/// \file
/// \brief Texture cache.

#ifndef __Y_RENDERER_TEXTURE_CACHE_H
#define __Y_RENDERER_TEXTURE_CACHE_H

#include <Yttrium/renderer.h>
#include <Yttrium/object.h>

#include <map>

namespace Yttrium
{

class ImageFormat;
class TextureCache;

/// Texture cache pointer.

typedef ObjectPointer<TextureCache> TextureCachePtr;

/// Texture cache.

class Y_API TextureCache
	: public Object
{
	TextureCache(const TextureCache &) = delete;
	TextureCache &operator =(const TextureCache &) = delete;

public:

	///

	TextureCache(const Renderer &renderer) noexcept;

	///

	virtual ~TextureCache() noexcept;

public:

	///

	Texture2D cache_texture_2d(const StaticString &name) noexcept;

	/// Clear the cache.

	void clear() noexcept;

	///

	Texture2D load_texture_2d(const StaticString &name) noexcept;

public:

	///

	static TextureCachePtr create(const Renderer &renderer) noexcept;

protected:

	virtual Texture2D::Private *cache_texture_2d(const ImageFormat &format, const void *data) noexcept = 0;

protected:

	Renderer _renderer;

private:

	typedef std::map<String, Texture2D> Cache2D;

	Cache2D  _cache_2d;
};

} // namespace Yttrium

#endif // __Y_RENDERER_TEXTURE_CACHE_H
