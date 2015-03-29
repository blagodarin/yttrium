/// \file
/// \brief Texture cache.

#ifndef __Y_RENDERER_TEXTURE_CACHE_H
#define __Y_RENDERER_TEXTURE_CACHE_H

#include <yttrium/renderer/pointers.h>

namespace Yttrium
{
	class StaticString;

	/// Texture cache.
	class TextureCache
	{
	public:

		TextureCache() = default;
		virtual ~TextureCache() = default;

		///
		virtual Texture2DPtr cache_texture_2d(const StaticString &name, bool intensity = false) = 0;

		/// Clear the cache.
		virtual void clear() = 0;

		///
		virtual Texture2DPtr load_texture_2d(const StaticString &name, bool intensity = false) = 0;
	};
}

#endif // __Y_RENDERER_TEXTURE_CACHE_H
