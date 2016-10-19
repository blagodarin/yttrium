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
#include <yttrium/std/map.h>

#include <mutex>

namespace Yttrium
{
	// TODO: Shared mutexes.
	// TODO: Deferred resource deallocation.

	class ResourceLoaderPrivate
	{
	public:
		ResourceLoaderPrivate(const Storage& storage, Renderer* renderer, AudioManager* audio_manager, Allocator& allocator)
			: _storage(storage)
			, _renderer(renderer)
			, _audio_manager(audio_manager)
			, _allocator(allocator)
		{
		}

	public:
		const Storage& _storage;
		Renderer* const _renderer = nullptr;
		AudioManager* const _audio_manager = nullptr;
		Allocator& _allocator;
		StdMap<String, std::weak_ptr<const Sound>> _sounds{ _allocator };
		std::mutex _sounds_mutex;
	};

	ResourceLoader::ResourceLoader(const Storage& storage, Renderer* renderer, AudioManager* audio_manager, Allocator& allocator)
		: _private(std::make_unique<ResourceLoaderPrivate>(storage, renderer, audio_manager, allocator))
	{
	}

	std::unique_ptr<const IonDocument> ResourceLoader::load_ion(const StaticString& name) const
	{
		auto ion = std::make_unique<IonDocument>(_private->_allocator);
		return ion->load(_private->_storage.open(name)) ? std::move(ion) : nullptr;
	}

	std::shared_ptr<const Sound> ResourceLoader::load_sound(const StaticString& name)
	{
		if (!_private->_audio_manager)
			return {};
		std::lock_guard<std::mutex> lock(_private->_sounds_mutex);
		const auto i = _private->_sounds.find(String(name, ByReference()));
		if (i != _private->_sounds.end())
		{
			auto sound = i->second.lock();
			if (sound)
				return sound;
		}
		auto sound = _private->_audio_manager->create_sound(name);
		if (i != _private->_sounds.end())
			i->second = sound;
		else
			_private->_sounds.emplace(String(name, &_private->_allocator), sound);
		return sound;
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

	SharedPtr<TextureFont> ResourceLoader::load_texture_font(const StaticString& name)
	{
		try
		{
			return TextureFont::load(_private->_storage.open(name), _private->_allocator);
		}
		catch (const ResourceError& e)
		{
			Log() << "Can't load \""_s << name << "\": "_s << e.what();
			return {};
		}
	}

	std::unique_ptr<const Translation> ResourceLoader::load_translation(const StaticString& name) const
	{
		return Translation::open(_private->_storage.open(name), _private->_allocator);
	}

	ResourceLoader::~ResourceLoader() = default;
}
