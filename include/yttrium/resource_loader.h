#ifndef _include_yttrium_resource_loader_h_
#define _include_yttrium_resource_loader_h_

#include <yttrium/api.h>

#include <memory>
#include <string_view>

namespace Yttrium
{
	class AudioManager;
	class Material;
	class Mesh;
	class MusicReader;
	class RenderManager;
	class Sound;
	class Source;
	class Storage;
	class Texture2D;
	class TextureFont;
	class Translation;

	///
	class Y_ENGINE_API ResourceLoader
	{
	public:
		///
		ResourceLoader(const Storage&, RenderManager* = nullptr, AudioManager* = nullptr);

		///
		~ResourceLoader();

		/// Loads a resource by name (if it isn't already loaded) and returns a pointer to the loaded resource.
		/// Throws ResourceError if there is no such resource.
		/// Throws DataError if the resource can't be loaded.
		std::shared_ptr<const Material> load_material(std::string_view name);
		std::shared_ptr<const Mesh> load_mesh(std::string_view name);
		std::shared_ptr<const Sound> load_sound(std::string_view name);
		std::shared_ptr<const Texture2D> load_texture_2d(std::string_view name);
		std::shared_ptr<const TextureFont> load_texture_font(std::string_view name);
		std::shared_ptr<const Translation> load_translation(std::string_view name);

		///
		std::unique_ptr<Source> open(std::string_view name);
		std::unique_ptr<MusicReader> open_music(std::string_view name);

		/// Releases all loaded resources that have no references outside of the loader.
		void release_unused();

	private:
		const std::unique_ptr<class ResourceLoaderPrivate> _private;
	};
}

#endif
