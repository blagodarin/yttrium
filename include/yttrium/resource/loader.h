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
	template <typename> class SharedPtr;
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
		std::unique_ptr<const IonDocument> load_ion(const StaticString& name) const; // TODO: Consider throwing on failure or returning optional.

		///
		std::shared_ptr<const Sound> load_sound(const StaticString& name);

		///
		std::shared_ptr<Texture2D> load_texture_2d(const StaticString& name, bool intensity = false);

		///
		SharedPtr<TextureFont> load_texture_font(const StaticString& name);

		///
		std::unique_ptr<const Translation> load_translation(const StaticString& name) const;

	private:
		const std::unique_ptr<class ResourceLoaderPrivate> _private;

	public:
		~ResourceLoader();
	};
}

#endif
