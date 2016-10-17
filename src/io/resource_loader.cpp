#include <yttrium/io/resource_loader.h>

#include <yttrium/audio/manager.h>
#include <yttrium/audio/sound.h>
#include <yttrium/image.h>
#include <yttrium/io/reader.h>
#include <yttrium/io/storage.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>

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

	const Storage& ResourceLoader::storage() const
	{
		return _private->_storage;
	}

	ResourceLoader::~ResourceLoader() = default;
}
