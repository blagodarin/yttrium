#include <yttrium/resources/resource_loader.h>

#include <yttrium/audio/manager.h>
#include <yttrium/audio/sound.h>
#include <yttrium/exceptions.h>
#include <yttrium/image.h>
#include <yttrium/io/reader.h>
#include <yttrium/io/storage.h>
#include <yttrium/ion/document.h>
#include <yttrium/log.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/resources/texture_font.h>
#include <yttrium/resources/translation.h>
#include <yttrium/std/map.h>

#include <mutex>

namespace Yttrium
{
	// TODO: Count loaded resources and resource loads.
	// TODO: Shared mutexes.

	template <typename T>
	struct ResourceCache
	{
		Allocator& _allocator;
		StdMap<String, ResourcePtr<const T>> _map{ _allocator };
		std::mutex _mutex;

		ResourceCache(Allocator& allocator) : _allocator(allocator) {}

		void collect()
		{
			std::vector<ResourcePtr<const T>> collected;
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto i = _map.begin(); i != _map.end(); )
			{
				if (i->second.unique())
				{
					collected.emplace_back(std::move(i->second));
					i = _map.erase(i);
				}
				else
					++i;
			}
		}

		ResourcePtr<const T> fetch(const StaticString& name, const std::function<ResourcePtr<const T>(const StaticString&)>& factory)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			const auto i = _map.find(String(name, ByReference()));
			if (i != _map.end())
				return i->second;
			auto resource_ptr = factory(name);
			if (!resource_ptr)
				return {}; // TODO: Throw.
			_map.emplace(String(name, &_allocator), resource_ptr);
			return resource_ptr;
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
		ResourceCache<Translation> _translations{ _allocator };
	};

	ResourceLoader::ResourceLoader(const Storage& storage, Renderer* renderer, AudioManager* audio_manager, Allocator& allocator)
		: _private(std::make_unique<ResourceLoaderPrivate>(storage, renderer, audio_manager, allocator))
	{
	}

	void ResourceLoader::collect()
	{
		_private->_sounds.collect();
		_private->_textures_2d.collect();
		_private->_texture_fonts.collect();
		_private->_translations.collect();
	}

	std::unique_ptr<const IonDocument> ResourceLoader::load_ion(const StaticString& name) const
	{
		auto ion = std::make_unique<IonDocument>(_private->_allocator);
		return ion->load(_private->_storage.open(name)) ? std::move(ion) : nullptr;
	}

	ResourcePtr<const Sound> ResourceLoader::load_sound(const StaticString& name)
	{
		if (!_private->_audio_manager)
			return {};
		return _private->_sounds.fetch(name, [this](const StaticString& name)
		{
			return _private->_audio_manager->create_sound(_private->_storage.open(name));
		});
	}

	ResourcePtr<const Texture2D> ResourceLoader::load_texture_2d(const StaticString& name, bool intensity)
	{
		// TODO: Map texture memory, then read the image into that memory.
		if (!_private->_renderer)
			return {};
		return _private->_textures_2d.fetch(name, [this, intensity](const StaticString& name) -> ResourcePtr<const Texture2D>
		{
			Image image;
			if (!image.load(_private->_storage.open(name)))
				return {};
			if (intensity && image.format().pixel_format() == PixelFormat::Gray)
				image.intensity_to_bgra();
			return _private->_renderer->create_texture_2d(image.format(), image.data());
		});
	}

	ResourcePtr<const TextureFont> ResourceLoader::load_texture_font(const StaticString& name)
	{
		return _private->_texture_fonts.fetch(name, [this](const StaticString& name) -> ResourcePtr<const TextureFont>
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

	ResourcePtr<const Translation> ResourceLoader::load_translation(const StaticString& name)
	{
		return _private->_translations.fetch(name, [this](const StaticString& name)
		{
			return Translation::open(_private->_storage.open(name), _private->_allocator);
		});
	}

	ResourceLoader::~ResourceLoader() = default;
}
