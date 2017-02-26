#include <yttrium/resource_loader.h>

#include <yttrium/audio/manager.h>
#include <yttrium/audio/music.h>
#include <yttrium/audio/sound.h>
#include <yttrium/exceptions.h>
#include <yttrium/gui/texture_font.h>
#include <yttrium/image.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/storage.h>
#include <yttrium/string.h>
#include <yttrium/translation.h>
#include "renderer/material.h"

#include <mutex>

namespace Yttrium
{
	template <typename T>
	struct ResourceCache
	{
		const Storage& _storage;
		std::map<String, std::shared_ptr<const T>> _map;
		std::mutex _mutex;

		ResourceCache(const Storage& storage)
			: _storage(storage)
		{
		}

		std::shared_ptr<const T> fetch(const StaticString& name, const std::function<std::shared_ptr<const T>(Reader&&)>& factory)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			const auto i = _map.find({ name, ByReference{} });
			if (i != _map.end())
				return i->second;
			auto reader = _storage.open(name);
			if (!reader)
				throw ResourceError("Can't find \"", name, "\"");
			auto resource_ptr = factory(std::move(reader));
			if (!resource_ptr)
				throw DataError("Can't load \"", name, "\""); // We don't have more information at this point. =(
			_map.emplace(String{ name }, resource_ptr);
			return resource_ptr;
		}

		bool release_unused()
		{
			std::vector<std::shared_ptr<const T>> unused;
			std::lock_guard<std::mutex> lock(_mutex);
			size_t released = 0;
			for (auto i = _map.begin(); i != _map.end(); )
			{
				if (i->second.unique())
				{
					unused.emplace_back(std::move(i->second));
					i = _map.erase(i);
					++released;
				}
				else
					++i;
			}
			return released > 0;
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

		bool release_unused()
		{
			return _material_cache.release_unused() // Uses textures.
				|| _ion_document_cache.release_unused()
				|| _mesh_cache.release_unused()
				|| _music_cache.release_unused()
				|| _sound_cache.release_unused()
				|| _texture_2d_cache.release_unused()
				|| _texture_font_cache.release_unused()
				|| _translation_cache.release_unused();
		}

	public:
		const Storage& _storage;
		Renderer* const _renderer = nullptr;
		AudioManager* const _audio_manager = nullptr;
		Allocator& _allocator;
		ResourceCache<IonDocument> _ion_document_cache{ _storage };
		ResourceCache<Material> _material_cache{ _storage };
		ResourceCache<Mesh> _mesh_cache{ _storage };
		ResourceCache<Music> _music_cache{ _storage };
		ResourceCache<Sound> _sound_cache{ _storage };
		ResourceCache<Texture2D> _texture_2d_cache{ _storage };
		ResourceCache<TextureFont> _texture_font_cache{ _storage };
		ResourceCache<Translation> _translation_cache{ _storage };
	};

	ResourceLoader::ResourceLoader(const Storage& storage, Renderer* renderer, AudioManager* audio_manager, Allocator& allocator)
		: _private(std::make_unique<ResourceLoaderPrivate>(storage, renderer, audio_manager, allocator))
	{
	}

	ResourceLoader::~ResourceLoader() = default;

	std::shared_ptr<const IonDocument> ResourceLoader::load_ion(const StaticString& name)
	{
		return _private->_ion_document_cache.fetch(name, [this](Reader&& reader)
		{
			return IonDocument::open(reader);
		});
	}

	std::shared_ptr<const Material> ResourceLoader::load_material(const StaticString& name)
	{
		if (!_private->_renderer)
			return {};
		return _private->_material_cache.fetch(name, [this, name](Reader&& reader) -> std::shared_ptr<const Material>
		{
			const auto document = IonDocument::open(reader);
			if (!document)
				return {};
			StaticString vertex_shader;
			StaticString fragment_shader;
			std::shared_ptr<const Texture2D> texture;
			for (const auto& node : document->root())
			{
				if (node.name() == "vertex_shader"_s)
				{
					if (node.size() != 1 || node.begin()->type() != IonValue::Type::String || node.begin()->string().is_empty())
						throw DataError("("_s, name, ") Bad 'vertex_shader'"_s);
					if (!vertex_shader.is_empty())
						throw DataError("("_s, name, ") Duplicate 'vertex_shader'"_s);
					vertex_shader = node.begin()->string();
				}
				else if (node.name() == "fragment_shader"_s)
				{
					if (node.size() != 1 || node.begin()->type() != IonValue::Type::String || node.begin()->string().is_empty())
						throw DataError("("_s, name, ") Bad 'fragment_shader'"_s);
					if (!fragment_shader.is_empty())
						throw DataError("("_s, name, ") Duplicate 'fragment_shader'"_s);
					fragment_shader = node.begin()->string();
				}
				else if (node.name() == "texture"_s)
				{
					if (node.size() != 1 || node.begin()->type() != IonValue::Type::String || node.begin()->string().is_empty())
						throw DataError("("_s, name, ") Bad 'texture'"_s);
					if (texture)
						throw DataError("("_s, name, ") Duplicate 'texture'"_s);
					texture = load_texture_2d(node.begin()->string());
				}
				else
					throw DataError("("_s, name, ") Bad material"_s);
			}
			if (vertex_shader.is_empty() || fragment_shader.is_empty())
				throw DataError("("_s, name, ") No 'vertex_shader' or 'fragment_shader'"_s);
			auto program = _private->_renderer->create_gpu_program(_private->_storage.open(vertex_shader).to_string(), _private->_storage.open(fragment_shader).to_string());
			if (!program)
				throw DataError("("_s, name, ") Bad 'vertex_shader' or 'fragment_shader'"_s);
			return std::make_shared<MaterialImpl>(std::move(program), std::move(texture));
		});
	}

	std::shared_ptr<const Mesh> ResourceLoader::load_mesh(const StaticString& name)
	{
		if (!_private->_renderer)
			return {};
		return _private->_mesh_cache.fetch(name, [this](Reader&& reader)
		{
			return _private->_renderer->load_mesh(std::move(reader));
		});
	}

	std::shared_ptr<const Music> ResourceLoader::load_music(const StaticString& name)
	{
		return _private->_music_cache.fetch(name, [this](Reader&& reader)
		{
			Music::Settings settings;
			settings.start = reader.property("start"_s).to_time();
			settings.end = reader.property("end"_s).to_time();
			settings.loop = reader.property("loop"_s).to_time();
			auto music = Music::open(std::move(reader));
			music->set_settings(settings);
			return music;
		});
	}

	std::shared_ptr<const Sound> ResourceLoader::load_sound(const StaticString& name)
	{
		if (!_private->_audio_manager)
			return {};
		return _private->_sound_cache.fetch(name, [this](Reader&& reader)
		{
			return _private->_audio_manager->create_sound(std::move(reader));
		});
	}

	std::shared_ptr<const Texture2D> ResourceLoader::load_texture_2d(const StaticString& name)
	{
		if (!_private->_renderer)
			return {};
		return _private->_texture_2d_cache.fetch(name, [this](Reader&& reader) -> std::shared_ptr<const Texture2D>
		{
			const bool intensity = reader.property("intensity"_s) == "1"_s;
			// TODO: Map texture memory, then read the image into that memory.
			auto image = Image::load(std::move(reader));
			if (!image)
				return {};
			return _private->_renderer->create_texture_2d(std::move(*image), intensity ? Renderer::TextureFlag::Intensity : Flags<Renderer::TextureFlag>{});
		});
	}

	std::shared_ptr<const TextureFont> ResourceLoader::load_texture_font(const StaticString& name)
	{
		return _private->_texture_font_cache.fetch(name, [this](Reader&& reader)
		{
			return TextureFont::open(std::move(reader));
		});
	}

	std::shared_ptr<const Translation> ResourceLoader::load_translation(const StaticString& name)
	{
		return _private->_translation_cache.fetch(name, [this](Reader&& reader)
		{
			return Translation::open(reader);
		});
	}

	void ResourceLoader::release_unused()
	{
		_private->release_unused();
		assert(!_private->release_unused());
	}
}