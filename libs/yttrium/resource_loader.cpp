#include <yttrium/resource_loader.h>

#include <yttrium/audio/manager.h>
#include <yttrium/audio/music_reader.h>
#include <yttrium/audio/sound.h>
#include <yttrium/exceptions.h>
#include <yttrium/gui/texture_font.h>
#include <yttrium/image.h>
#include <yttrium/ion/reader.h>
#include <yttrium/renderer/manager.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/program.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/storage.h>
#include <yttrium/translation.h>
#include "gui/ion/loader.h"
#include "renderer/material.h"
#include "utils/string.h"

#include <cassert>
#include <mutex>

namespace
{
	// https://developercommunity.visualstudio.com/content/problem/74313/compiler-error-in-xsmf-control-with-stdoptional-pa.html
	Yttrium::Image load_image(const Yttrium::Source& source)
	{
		auto image = Yttrium::Image::load(source);
		if (!image)
			throw Yttrium::DataError{"Can't load \"", source.name(), "\""};
		return std::move(*image);
	}
}

namespace Yttrium
{
	template <typename T>
	struct ResourceCache
	{
		const Storage& _storage;
		std::map<std::string, std::shared_ptr<const T>, std::less<>> _map;
		std::mutex _mutex;

		explicit ResourceCache(const Storage& storage) : _storage{storage} {}

		std::shared_ptr<const T> fetch(std::string_view name, const std::function<std::shared_ptr<const T>(std::unique_ptr<Source>&&)>& factory)
		{
			std::lock_guard<std::mutex> lock{_mutex};
			const auto i = _map.find(name);
			if (i != _map.end())
				return i->second;
			auto source = _storage.open(name);
			if (!source)
				throw ResourceError{"Can't find \"", name, "\""};
			auto resource_ptr = factory(std::move(source));
			if (!resource_ptr)
				throw DataError{"Can't load \"", name, "\""}; // We don't have more information at this point. =(
			_map.emplace(name, resource_ptr);
			return resource_ptr;
		}

		auto release_unused()
		{
			std::vector<std::shared_ptr<const T>> unused;
			std::lock_guard<std::mutex> lock{_mutex};
			size_t released = 0;
			for (auto i = _map.begin(); i != _map.end(); )
			{
				if (i->second.use_count() == 1)
				{
					unused.emplace_back(std::move(i->second));
					i = _map.erase(i);
					++released;
				}
				else
					++i;
			}
			return released;
		}
	};

	class ResourceLoaderPrivate
	{
	public:
		ResourceLoaderPrivate(const Storage& storage, RenderManager* render_manager, AudioManager* audio_manager)
			: _storage{storage}, _render_manager{render_manager}, _audio_manager{audio_manager} {}

		bool release_unused()
		{
			auto released = _material_cache.release_unused(); // Uses textures.
			released += _mesh_cache.release_unused();
			released += _sound_cache.release_unused();
			released += _texture_2d_cache.release_unused();
			released += _texture_font_cache.release_unused();
			released += _translation_cache.release_unused();
			return released > 0;
		}

	public:
		const Storage& _storage;
		RenderManager* const _render_manager = nullptr;
		AudioManager* const _audio_manager = nullptr;
		ResourceCache<Material> _material_cache{_storage};
		ResourceCache<Mesh> _mesh_cache{_storage};
		ResourceCache<Sound> _sound_cache{_storage};
		ResourceCache<Texture2D> _texture_2d_cache{_storage};
		ResourceCache<TextureFont> _texture_font_cache{_storage};
		ResourceCache<Translation> _translation_cache{_storage};
	};

	ResourceLoader::ResourceLoader(const Storage& storage, RenderManager* render_manager, AudioManager* audio_manager)
		: _private{std::make_unique<ResourceLoaderPrivate>(storage, render_manager, audio_manager)}
	{
	}

	ResourceLoader::~ResourceLoader() = default;

	std::shared_ptr<const Material> ResourceLoader::load_material(std::string_view name)
	{
		if (!_private->_render_manager)
			return {};
		return _private->_material_cache.fetch(name, [this, name](std::unique_ptr<Source>&& source) -> std::shared_ptr<const Material>
		{
			IonReader ion{*source};
			std::unique_ptr<Source> vertex_shader;
			std::unique_ptr<Source> fragment_shader;
			std::shared_ptr<const Texture2D> texture;
			Texture2D::Filter texture_filter = Texture2D::NearestFilter;
			for (auto token = ion.read(); token.type() != IonReader::Token::Type::End;)
			{
				const auto ion_name = token.to_name();
				const auto make_location = [name, &token]
				{
					return make_string("(", name, ":", token.line(), ":", token.column(), ") ");
				};
				if (ion_name == "vertex_shader")
				{
					if (vertex_shader)
						throw DataError{make_location(), "Duplicate 'vertex_shader'"};
					vertex_shader = _private->_storage.open(ion.read().to_value());
					if (!vertex_shader)
						throw DataError{make_location(), "Bad 'vertex_shader'"};
					token = ion.read();
				}
				else if (ion_name == "fragment_shader")
				{
					if (fragment_shader)
						throw DataError{make_location(), "Duplicate 'fragment_shader'"};
					fragment_shader = _private->_storage.open(ion.read().to_value());
					if (!fragment_shader)
						throw DataError{make_location(), "Bad 'fragment_shader'"};
					token = ion.read();
				}
				else if (ion_name == "texture")
				{
					if (texture)
						throw DataError{make_location(), "Duplicate 'texture'"};
					const auto texture_name = ion.read().to_value();
					if (texture_name.empty())
						throw DataError{make_location(), "Bad 'texture'"};
					if (!GuiIonLoader::read_texture_filter(ion, token.next(ion), texture_filter))
						throw DataError{make_location(), "Bad texture property"};
					texture = load_texture_2d(texture_name);
				}
				else
					throw DataError{make_location(), "Unknown material entry '", ion_name, "'"};
			}
			if (!vertex_shader)
				throw DataError{"(", name, ") No 'vertex_shader'"};
			if (!fragment_shader)
				throw DataError{"(", name, ") No 'fragment_shader'"};
			auto program = _private->_render_manager->create_program(vertex_shader->to_string(), fragment_shader->to_string());
			if (!program)
				throw DataError{"(", name, ") Bad 'vertex_shader' or 'fragment_shader'"};
			return std::make_shared<MaterialImpl>(std::move(program), std::move(texture), texture_filter);
		});
	}

	std::shared_ptr<const Mesh> ResourceLoader::load_mesh(std::string_view name)
	{
		if (!_private->_render_manager)
			return {};
		return _private->_mesh_cache.fetch(name, [this](std::unique_ptr<Source>&& source)
		{
			return _private->_render_manager->load_mesh(*source);
		});
	}

	std::shared_ptr<const Sound> ResourceLoader::load_sound(std::string_view name)
	{
		if (!_private->_audio_manager)
			return {};
		return _private->_sound_cache.fetch(name, [this](std::unique_ptr<Source>&& source)
		{
			return _private->_audio_manager->create_sound(std::move(source));
		});
	}

	std::shared_ptr<const Texture2D> ResourceLoader::load_texture_2d(std::string_view name)
	{
		if (!_private->_render_manager)
			return {};
		return _private->_texture_2d_cache.fetch(name, [this](std::unique_ptr<Source>&& source) -> std::shared_ptr<const Texture2D>
		{
			Flags<RenderManager::TextureFlag> flags;
			if (source->property("intensity") == "1")
				flags |= RenderManager::TextureFlag::Intensity;
			return _private->_render_manager->create_texture_2d(::load_image(*source), flags);
		});
	}

	std::shared_ptr<const TextureFont> ResourceLoader::load_texture_font(std::string_view name)
	{
		return _private->_texture_font_cache.fetch(name, [](std::unique_ptr<Source>&& source)
		{
			return TextureFont::load(*source);
		});
	}

	std::shared_ptr<const Translation> ResourceLoader::load_translation(std::string_view name)
	{
		return _private->_translation_cache.fetch(name, [](std::unique_ptr<Source>&& source)
		{
			return Translation::load(*source);
		});
	}

	std::unique_ptr<Source> ResourceLoader::open(std::string_view name)
	{
		auto source = _private->_storage.open(name);
		if (!source)
			throw ResourceError{"Missing \"", name, "\""};
		return source;
	}

	std::unique_ptr<MusicReader> ResourceLoader::open_music(std::string_view name)
	{
		auto source = open(name);

		const auto start_ms = time_from_chars(source->property("start"));
		const auto end_ms = time_from_chars(source->property("end"));
		const auto loop_ms = time_from_chars(source->property("loop"));

		auto music = MusicReader::open(std::move(source));
		if (!music->set_properties(start_ms, end_ms, loop_ms))
			throw DataError{"(", name, ") Bad music properties"};
		return music;
	}

	void ResourceLoader::release_unused()
	{
		_private->release_unused();
		assert(!_private->release_unused());
	}
}
