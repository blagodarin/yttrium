/// \file
/// \brief

#ifndef _include_yttrium_resource_loader_h_
#define _include_yttrium_resource_loader_h_

#include <yttrium/memory/global.h>

#include <memory>

namespace Yttrium
{
	class AudioManager;
	class IonDocument;
	class Renderer;
	template <typename> class ResourcePtr;
	class Sound;
	class StaticString;
	class Storage;
	class Texture2D;
	class TextureFont;
	class Translation;

	///
	class Y_API ResourceLoader
	{
	public:
		///
		ResourceLoader(const Storage&, Renderer* = nullptr, AudioManager* = nullptr, Allocator& = *DefaultAllocator);

		/// Releases all loaded resources that have no references outside of the loader.
		void collect();

		///
		std::unique_ptr<const IonDocument> load_ion(const StaticString& name) const;

		///
		ResourcePtr<const Sound> load_sound(const StaticString& name);

		///
		ResourcePtr<const Texture2D> load_texture_2d(const StaticString& name, bool intensity = false); // TODO: Make parameter-less.

		///
		ResourcePtr<const TextureFont> load_texture_font(const StaticString& name);

		///
		ResourcePtr<const Translation> load_translation(const StaticString& name);

	private:
		const std::unique_ptr<class ResourceLoaderPrivate> _private;

	public:
		~ResourceLoader();
	};
}

#endif
