#include <yttrium/resources/resource_loader.h>

#include <yttrium/audio/manager.h>
#include <yttrium/audio/sound.h>
#include <yttrium/exceptions.h>
#include <yttrium/gui/texture_font.h>
#include <yttrium/image.h>
#include <yttrium/ion/document.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/resources/resource_ptr.h>
#include <yttrium/resources/translation.h>
#include <yttrium/std/map.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/storage.h>

#include <mutex>

namespace Yttrium
{
	// TODO: Count loaded resources and resource loads.
	// TODO: Shared mutexes.

	template <typename T>
	struct ResourceCache
	{
		const Storage& _storage;
		Allocator& _allocator;
		StdMap<String, ResourcePtr<const T>> _map{ _allocator };
		std::mutex _mutex;

		ResourceCache(const Storage& storage, Allocator& allocator)
			: _storage(storage)
			, _allocator(allocator)
		{
		}

		ResourcePtr<const T> fetch(const StaticString& name, const std::function<ResourcePtr<const T>(Reader&&)>& factory)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			const auto i = _map.find(String(name, ByReference()));
			if (i != _map.end())
				return i->second;
			auto reader = _storage.open(name);
			if (!reader)
				throw ResourceError("Can't find \"", name, "\"");
			auto resource_ptr = factory(std::move(reader));
			if (!resource_ptr)
				throw DataError("Can't load \"", name, "\""); // We don't have more information at this point. =(
			_map.emplace(String(name, &_allocator), resource_ptr);
			return resource_ptr;
		}

		void release_unused()
		{
			std::vector<ResourcePtr<const T>> unused;
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto i = _map.begin(); i != _map.end(); )
			{
				if (i->second.unique())
				{
					unused.emplace_back(std::move(i->second));
					i = _map.erase(i);
				}
				else
					++i;
			}
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
		ResourceCache<IonDocument> _ion_document_cache{ _storage, _allocator };
		ResourceCache<Sound> _sound_cache{ _storage, _allocator };
		ResourceCache<Texture2D> _texture_2d_cache{ _storage, _allocator };
		ResourceCache<TextureFont> _texture_font_cache{ _storage, _allocator };
		ResourceCache<Translation> _translation_cache{ _storage, _allocator };
	};

	ResourceLoader::ResourceLoader(const Storage& storage, Renderer* renderer, AudioManager* audio_manager, Allocator& allocator)
		: _private(std::make_unique<ResourceLoaderPrivate>(storage, renderer, audio_manager, allocator))
	{
	}

	ResourcePtr<const IonDocument> ResourceLoader::load_ion(const StaticString& name)
	{
		return _private->_ion_document_cache.fetch(name, [this](Reader&& reader)
		{
			return IonDocument::open(reader, _private->_allocator);
		});
	}

	ResourcePtr<const Sound> ResourceLoader::load_sound(const StaticString& name)
	{
		if (!_private->_audio_manager)
			return {};
		return _private->_sound_cache.fetch(name, [this](Reader&& reader)
		{
			return _private->_audio_manager->create_sound(std::move(reader));
		});
	}

	ResourcePtr<const Texture2D> ResourceLoader::load_texture_2d(const StaticString& name, bool intensity)
	{
		if (!_private->_renderer)
			return {};
		return _private->_texture_2d_cache.fetch(name, [this, intensity](Reader&& reader) -> ResourcePtr<const Texture2D>
		{
			// TODO: Map texture memory, then read the image into that memory.
			Image image;
			if (!image.load(std::move(reader)))
				return {};
			if (intensity && image.format().pixel_format() == PixelFormat::Gray)
				image.intensity_to_bgra(); // TODO: Move this inside the renderer.
			return _private->_renderer->create_texture_2d(image.format(), image.data());
		});
	}

	ResourcePtr<const TextureFont> ResourceLoader::load_texture_font(const StaticString& name)
	{
		return _private->_texture_font_cache.fetch(name, [this](Reader&& reader)
		{
			return TextureFont::open(std::move(reader), _private->_allocator);
		});
	}

	ResourcePtr<const Translation> ResourceLoader::load_translation(const StaticString& name)
	{
		return _private->_translation_cache.fetch(name, [this](Reader&& reader)
		{
			return Translation::open(reader, _private->_allocator);
		});
	}

	void ResourceLoader::release_unused()
	{
		_private->_ion_document_cache.release_unused();
		_private->_sound_cache.release_unused();
		_private->_texture_2d_cache.release_unused();
		_private->_texture_font_cache.release_unused();
		_private->_translation_cache.release_unused();
	}

	ResourceLoader::~ResourceLoader() = default;
}
