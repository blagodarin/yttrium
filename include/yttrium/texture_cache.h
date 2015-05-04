/// \file
/// \brief Texture cache.

#ifndef __Y_TEXTURE_CACHE_H
#define __Y_TEXTURE_CACHE_H

#include <yttrium/global.h>

#include <memory>

namespace Yttrium
{
	class Renderer;
	class StaticString;
	class Texture2D;

	template <typename T>
	class Pointer;

	/// Texture cache.
	class Y_API TextureCache
	{
	public:

		///
		static std::unique_ptr<TextureCache> create(Renderer& renderer);

		TextureCache() = default;
		virtual ~TextureCache() = default;

		/// Clear the cache.
		virtual void clear() = 0;

		///
		virtual Pointer<Texture2D> load_texture_2d(const StaticString& name, bool intensity = false) = 0;
	};
}

#endif
