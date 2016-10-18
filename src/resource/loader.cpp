#include <yttrium/resource/loader.h>

#include <yttrium/audio/manager.h>
#include <yttrium/audio/sound.h>
#include <yttrium/image.h>
#include <yttrium/log.h>
#include <yttrium/i18n/translation.h>
#include <yttrium/io/errors.h>
#include <yttrium/io/reader.h>
#include <yttrium/io/storage.h>
#include <yttrium/ion/document.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/renderer/texture_font.h>

namespace Yttrium
{
	// TODO: Resource cache.

	class ResourceLoaderPrivate
	{
	public:
		ResourceLoaderPrivate(const Storage& storage, Renderer* renderer, AudioManager* audio_manager)
			: _storage(storage)
			, _renderer(renderer)
			, _audio_manager(audio_manager)
		{
		}

	public:
		const Storage& _storage;
		Renderer* const _renderer = nullptr;
		AudioManager* const _audio_manager = nullptr;
	};

	ResourceLoader::ResourceLoader(const Storage& storage, Renderer* renderer, AudioManager* audio_manager)
		: _private(std::make_unique<ResourceLoaderPrivate>(storage, renderer, audio_manager))
	{
	}

	std::unique_ptr<const IonDocument> ResourceLoader::load_ion(const StaticString& name, Allocator& allocator) const
	{
		auto ion = std::make_unique<IonDocument>(allocator);
		return ion->load(_private->_storage.open(name)) ? std::move(ion) : nullptr;
	}

	SharedPtr<Sound> ResourceLoader::load_sound(const StaticString& name)
	{
		return _private->_audio_manager ? _private->_audio_manager->create_sound(name) : SharedPtr<Sound>();
	}

	SharedPtr<Texture2D> ResourceLoader::load_texture_2d(const StaticString& name, bool intensity)
	{
		// TODO: Map texture memory, then read the image into that memory.
		if (!_private->_renderer)
			return {};
		Image image;
		if (!image.load(_private->_storage.open(name)))
			return {};
		if (intensity && image.format().pixel_format() == PixelFormat::Gray)
			image.intensity_to_bgra();
		return _private->_renderer->create_texture_2d(image.format(), image.data());
	}

	SharedPtr<TextureFont> ResourceLoader::load_texture_font(const StaticString& name, Allocator& allocator)
	{
		try
		{
			return TextureFont::load(_private->_storage.open(name), allocator);
		}
		catch (const ResourceError& e)
		{
			Log() << "Can't load \""_s << name << "\": "_s << e.what();
			return {};
		}
	}

	std::unique_ptr<const Translation> ResourceLoader::load_translation(const StaticString& name, Allocator& allocator) const
	{
		return Translation::open(_private->_storage.open(name), allocator);
	}

	ResourceLoader::~ResourceLoader() = default;
}
