/// \file
/// \brief Texture cache.

#ifndef _include_yttrium_renderer_texture_cache_h_
#define _include_yttrium_renderer_texture_cache_h_

#include <yttrium/global.h>

namespace Yttrium
{
	class Renderer;
	class StaticString;
	class Storage;
	class Texture2D;

	template <typename> class SharedPtr;
	template <typename> class UniquePtr;

	/// Texture cache.
	class Y_API TextureCache
	{
	public:
		///
		static UniquePtr<TextureCache> create(const Storage&, Renderer&);

		TextureCache() = default;
		virtual ~TextureCache() = default;

		/// Clear the cache.
		virtual void clear() = 0;

		///
		virtual SharedPtr<Texture2D> load_texture_2d(const StaticString& name, bool intensity = false) = 0;
	};
}

#endif
