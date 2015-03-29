/// \file
/// \brief Texture cache.

#ifndef __Y_RENDERER_TEXTURE_CACHE_H
#define __Y_RENDERER_TEXTURE_CACHE_H

namespace Yttrium
{
	class StaticString;
	class Texture2D;

	template <typename T>
	class Pointer;

	/// Texture cache.
	class TextureCache
	{
	public:

		TextureCache() = default;
		virtual ~TextureCache() = default;

		///
		virtual Pointer<Texture2D> cache_texture_2d(const StaticString &name, bool intensity = false) = 0;

		/// Clear the cache.
		virtual void clear() = 0;

		///
		virtual Pointer<Texture2D> load_texture_2d(const StaticString &name, bool intensity = false) = 0;
	};
}

#endif // __Y_RENDERER_TEXTURE_CACHE_H
