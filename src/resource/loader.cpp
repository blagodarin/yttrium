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

	template <typename T>
	struct ResourceCache
	{
		Allocator& _allocator;
		StdMap<String, std::weak_ptr<T>> _map{ _allocator };
		std::mutex _mutex;

		ResourceCache(Allocator& allocator) : _allocator(allocator) {}

		std::shared_ptr<T> fetch(const StaticString& name, const std::function<std::shared_ptr<T>(const StaticString&)>& factory)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			const auto i = _map.find(String(name, ByReference()));
			if (i != _map.end())
			{
				auto resource = i->second.lock();
				if (resource)
					return resource;
			}
			auto resource = factory(name);
			if (!resource)
				return {};
			if (i != _map.end())
				i->second = resource;
			else
				_map.emplace(String(name, &_allocator), resource);
			return resource;
		}
	};

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
		ResourceCache<Sound> _sounds{ _allocator };
		ResourceCache<Texture2D> _textures_2d{ _allocator };
		ResourceCache<TextureFont> _texture_fonts{ _allocator };
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
		return _private->_sounds.fetch(name, [this](const StaticString& name)
		{
			return _private->_audio_manager->create_sound(name);
		});
	}

	std::shared_ptr<Texture2D> ResourceLoader::load_texture_2d(const StaticString& name, bool intensity)
	{
		// TODO: Map texture memory, then read the image into that memory.
		if (!_private->_renderer)
			return {};
		return _private->_textures_2d.fetch(name, [this, intensity](const StaticString& name) -> std::shared_ptr<Texture2D>
		{
			Image image;
			if (!image.load(_private->_storage.open(name)))
				return {};
			if (intensity && image.format().pixel_format() == PixelFormat::Gray)
				image.intensity_to_bgra();
			return _private->_renderer->create_texture_2d(image.format(), image.data());
		});
	}

	std::shared_ptr<const TextureFont> ResourceLoader::load_texture_font(const StaticString& name)
	{
		return _private->_texture_fonts.fetch(name, [this](const StaticString& name) -> std::shared_ptr<TextureFont>
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
		});
	}

	std::unique_ptr<const Translation> ResourceLoader::load_translation(const StaticString& name) const
	{
		return Translation::open(_private->_storage.open(name), _private->_allocator);
	}

	ResourceLoader::~ResourceLoader() = default;
}
