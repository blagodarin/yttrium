/// \file
/// \brief

#ifndef _include_yttrium_io_resource_loader_h_
#define _include_yttrium_io_resource_loader_h_

#include <yttrium/global.h>

#include <memory>

namespace Yttrium
{
	class AudioManager;
	class Renderer;
	template <typename> class SharedPtr;
	class Sound;
	class StaticString;
	class Storage;
	class Texture2D;
	class TextureFont;

	///
	class Y_API ResourceLoader
	{
	public:
		///
		ResourceLoader(const Storage&, Renderer* = nullptr, AudioManager* = nullptr);

		///
		SharedPtr<Sound> load_sound(const StaticString& name);

		///
		SharedPtr<Texture2D> load_texture_2d(const StaticString& name, bool intensity = false);

		///
		SharedPtr<TextureFont> load_texture_font(const StaticString& name);

		///
		const Storage& storage() const; // TODO: Remove.

	private:
		const std::unique_ptr<class ResourceLoaderPrivate> _private;

	public:
		~ResourceLoader();
	};
}

#endif
