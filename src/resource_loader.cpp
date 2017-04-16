#include <yttrium/resource_loader.h>

#include <yttrium/audio/manager.h>
#include <yttrium/audio/music.h>
#include <yttrium/audio/sound.h>
#include <yttrium/exceptions.h>
#include <yttrium/gui/texture_font.h>
#include <yttrium/image.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/reader.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/storage/source.h>
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

		explicit ResourceCache(const Storage& storage) : _storage{storage} {}

		std::shared_ptr<const T> fetch(const StaticString& name, const std::function<std::shared_ptr<const T>(std::unique_ptr<Source>&&)>& factory)
		{
			std::lock_guard<std::mutex> lock{_mutex};
			const auto i = _map.find({name, ByReference{}});
			if (i != _map.end())
				return i->second;
			auto source = _storage.open(name);
			if (!source)
				throw ResourceError{"Can't find \"", name, "\""};
			auto resource_ptr = factory(std::move(source));
			if (!resource_ptr)
				throw DataError{"Can't load \"", name, "\""}; // We don't have more information at this point. =(
			_map.emplace(String{name}, resource_ptr);
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
		ResourceLoaderPrivate(const Storage& storage, Renderer* renderer, AudioManager* audio_manager)
			: _storage{storage}, _renderer{renderer}, _audio_manager{audio_manager} {}

		bool release_unused()
		{
			auto released = _material_cache.release_unused(); // Uses textures.
			released += _ion_document_cache.release_unused();
			released += _mesh_cache.release_unused();
			released += _music_cache.release_unused();
			released += _sound_cache.release_unused();
			released += _texture_2d_cache.release_unused();
			released += _texture_font_cache.release_unused();
			released += _translation_cache.release_unused();
			return released > 0;
		}

	public:
		const Storage& _storage;
		Renderer* const _renderer = nullptr;
		AudioManager* const _audio_manager = nullptr;
		ResourceCache<IonDocument> _ion_document_cache{_storage};
		ResourceCache<Material> _material_cache{_storage};
		ResourceCache<Mesh> _mesh_cache{_storage};
		ResourceCache<Music> _music_cache{_storage};
		ResourceCache<Sound> _sound_cache{_storage};
		ResourceCache<Texture2D> _texture_2d_cache{_storage};
		ResourceCache<TextureFont> _texture_font_cache{_storage};
		ResourceCache<Translation> _translation_cache{_storage};
	};

	ResourceLoader::ResourceLoader(const Storage& storage, Renderer* renderer, AudioManager* audio_manager)
		: _private{std::make_unique<ResourceLoaderPrivate>(storage, renderer, audio_manager)}
	{
	}

	ResourceLoader::~ResourceLoader() = default;

	std::shared_ptr<const IonDocument> ResourceLoader::load_ion(const StaticString& name)
	{
		return _private->_ion_document_cache.fetch(name, [this](std::unique_ptr<Source>&& source)
		{
			return IonDocument::load(*source);
		});
	}

	std::shared_ptr<const Material> ResourceLoader::load_material(const StaticString& name)
	{
		if (!_private->_renderer)
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
					return make_string("("_s, name, ":"_s, token.line(), ":"_s, token.column(), ") "_s);
				};
				if (ion_name == "vertex_shader"_s)
				{
					if (vertex_shader)
						throw DataError{make_location(), "Duplicate 'vertex_shader'"_s};
					vertex_shader = _private->_storage.open(ion.read().to_value());
					if (!vertex_shader)
						throw DataError{make_location(), "Bad 'vertex_shader'"_s};
					token = ion.read();
				}
				else if (ion_name == "fragment_shader"_s)
				{
					if (fragment_shader)
						throw DataError{make_location(), "Duplicate 'fragment_shader'"_s};
					fragment_shader = _private->_storage.open(ion.read().to_value());
					if (!fragment_shader)
						throw DataError{make_location(), "Bad 'fragment_shader'"_s};
					token = ion.read();
				}
				else if (ion_name == "texture"_s)
				{
					if (texture)
						throw DataError{make_location(), "Duplicate 'texture'"_s};
					const auto texture_name = ion.read().to_value();
					if (texture_name.is_empty())
						throw DataError{make_location(), "Bad 'texture'"_s};
					token = ion.read();
					if (token.type() == IonReader::Token::Type::ObjectBegin) // TODO: Merge with GuiIonPropertyLoader::load_texture.
					{
						bool has_interpolation = false;
						bool has_anisotropy = false;
						for (token = ion.read(); token.type() != IonReader::Token::Type::ObjectEnd; token = ion.read())
						{
							const auto filter_option = token.to_name();
							if (filter_option == "nearest"_s)
							{
								if (has_interpolation)
									throw DataError{make_location(), "Bad texture property"_s};
								texture_filter = static_cast<Texture2D::Filter>(Texture2D::NearestFilter | (texture_filter & Texture2D::AnisotropicFilter));
								has_interpolation = true;
							}
							else if (filter_option == "linear"_s)
							{
								if (has_interpolation)
									throw DataError{make_location(), "Bad texture property"_s};
								texture_filter = static_cast<Texture2D::Filter>(Texture2D::LinearFilter | (texture_filter & Texture2D::AnisotropicFilter));
								has_interpolation = true;
							}
							else if (filter_option == "bilinear"_s)
							{
								if (has_interpolation)
									throw DataError{make_location(), "Bad texture property"_s};
								texture_filter = static_cast<Texture2D::Filter>(Texture2D::BilinearFilter | (texture_filter & Texture2D::AnisotropicFilter));
								has_interpolation = true;
							}
							else if (filter_option == "trilinear"_s)
							{
								if (has_interpolation)
									throw DataError{make_location(), "Bad texture property"_s};
								texture_filter = static_cast<Texture2D::Filter>(Texture2D::TrilinearFilter | (texture_filter & Texture2D::AnisotropicFilter));
								has_interpolation = true;
							}
							else if (filter_option == "anisotropic"_s)
							{
								if (!has_interpolation || has_anisotropy)
									throw DataError{make_location(), "Bad texture property"_s};
								texture_filter = static_cast<Texture2D::Filter>((texture_filter & Texture2D::IsotropicFilterMask) | Texture2D::AnisotropicFilter);
								has_anisotropy = true;
							}
							else
								throw DataError{make_location(), "Bad texture property"_s};
						}
						token = ion.read();
					}
					texture = load_texture_2d(texture_name);
				}
				else
					throw DataError{make_location(), "Unknown material entry '"_s, ion_name, "'"_s};
			}
			if (!vertex_shader)
				throw DataError{"("_s, name, ") No 'vertex_shader'"_s};
			if (!fragment_shader)
				throw DataError{"("_s, name, ") No 'fragment_shader'"_s};
			auto program = _private->_renderer->create_gpu_program(vertex_shader->to_string(), fragment_shader->to_string());
			if (!program)
				throw DataError{"("_s, name, ") Bad 'vertex_shader' or 'fragment_shader'"_s};
			return std::make_shared<MaterialImpl>(std::move(program), std::move(texture), texture_filter);
		});
	}

	std::shared_ptr<const Mesh> ResourceLoader::load_mesh(const StaticString& name)
	{
		if (!_private->_renderer)
			return {};
		return _private->_mesh_cache.fetch(name, [this](std::unique_ptr<Source>&& source)
		{
			return _private->_renderer->load_mesh(*source);
		});
	}

	std::shared_ptr<const Music> ResourceLoader::load_music(const StaticString& name)
	{
		return _private->_music_cache.fetch(name, [this](std::unique_ptr<Source>&& source)
		{
			Music::Settings settings;
			settings.start = source->property("start"_s).to_time();
			settings.end = source->property("end"_s).to_time();
			settings.loop = source->property("loop"_s).to_time();
			auto music = Music::open(std::move(source));
			music->set_settings(settings);
			return music;
		});
	}

	std::shared_ptr<const Sound> ResourceLoader::load_sound(const StaticString& name)
	{
		if (!_private->_audio_manager)
			return {};
		return _private->_sound_cache.fetch(name, [this](std::unique_ptr<Source>&& source)
		{
			return _private->_audio_manager->create_sound(std::move(source));
		});
	}

	std::shared_ptr<const Texture2D> ResourceLoader::load_texture_2d(const StaticString& name)
	{
		if (!_private->_renderer)
			return {};
		return _private->_texture_2d_cache.fetch(name, [this](std::unique_ptr<Source>&& source) -> std::shared_ptr<const Texture2D>
		{
			auto image = Image::load(*source);
			if (!image)
				return {};
			Flags<Renderer::TextureFlag> flags;
			if (source->property("intensity"_s) == "1"_s)
				flags |= Renderer::TextureFlag::Intensity;
			return _private->_renderer->create_texture_2d(std::move(*image), flags);
		});
	}

	std::shared_ptr<const TextureFont> ResourceLoader::load_texture_font(const StaticString& name)
	{
		return _private->_texture_font_cache.fetch(name, [this](std::unique_ptr<Source>&& source)
		{
			return TextureFont::load(*source);
		});
	}

	std::shared_ptr<const Translation> ResourceLoader::load_translation(const StaticString& name)
	{
		return _private->_translation_cache.fetch(name, [this](std::unique_ptr<Source>&& source)
		{
			return Translation::load(*source);
		});
	}

	std::unique_ptr<Source> ResourceLoader::open(const StaticString& name)
	{
		auto source = _private->_storage.open(name);
		if (!source)
			throw ResourceError{"Missing \"", name, "\""};
		return source;
	}

	void ResourceLoader::release_unused()
	{
		_private->release_unused();
		assert(!_private->release_unused());
	}
}
