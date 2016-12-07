/// \file
/// \brief

#ifndef _include_yttrium_resources_resource_loader_h_
#define _include_yttrium_resources_resource_loader_h_

#include <yttrium/memory/global.h>

#include <memory>

namespace Yttrium
{
	class AudioManager;
	class IonDocument;
	class Mesh;
	class Music;
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

		///
		~ResourceLoader();

		/// Loads a resource by name (if it isn't already loaded) and returns a pointer to the loaded resource.
		/// Throws ResourceError if there is no such resource.
		/// Throws DataError if the resource can't be loaded.
		ResourcePtr<const IonDocument> load_ion(const StaticString& name);
		ResourcePtr<const Mesh> load_mesh(const StaticString& name);
		ResourcePtr<const Music> load_music(const StaticString& name);
		ResourcePtr<const Sound> load_sound(const StaticString& name);
		ResourcePtr<const Texture2D> load_texture_2d(const StaticString& name);
		ResourcePtr<const TextureFont> load_texture_font(const StaticString& name);
		ResourcePtr<const Translation> load_translation(const StaticString& name);

		/// Releases all loaded resources that have no references outside of the loader.
		void release_unused();

	private:
		const std::unique_ptr<class ResourceLoaderPrivate> _private;
	};
}

#endif
