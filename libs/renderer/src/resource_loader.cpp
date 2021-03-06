// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/renderer/resource_loader.h>

#include <yttrium/base/exceptions.h>
#include <yttrium/image/image.h>
#include <yttrium/ion/reader.h>
#include <yttrium/renderer/manager.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/program.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/storage.h>
#include "material.h"

#include <cassert>
#include <map>
#include <mutex>
#include <functional>

#include <fmt/format.h>

namespace
{
	bool read_texture_filter(Yt::IonReader& ion, Yt::IonToken& token, Yt::Texture2D::Filter& filter)
	{
		static const std::unordered_map<std::string_view, Yt::Texture2D::Filter> filters{
			{ "nearest", Yt::Texture2D::NearestFilter },
			{ "linear", Yt::Texture2D::LinearFilter },
			{ "bilinear", Yt::Texture2D::BilinearFilter },
			{ "trilinear", Yt::Texture2D::TrilinearFilter },
		};

		if (token.type() != Yt::IonToken::Type::ObjectBegin)
			return true;
		// cppcheck-suppress stlIfFind
		if (const auto i = filters.find(token.next(ion).to_name()); i != filters.end())
			filter = i->second;
		else
			return false;
		if (token.next(ion).type() == Yt::IonToken::Type::Name && token.text() == "anisotropic")
		{
			filter = static_cast<Yt::Texture2D::Filter>(filter | Yt::Texture2D::AnisotropicFilter);
			token.next(ion);
		}
		token.check_object_end();
		token.next(ion);
		return true;
	}
}

namespace Yt
{
	template <typename T>
	struct ResourceCache
	{
		const Storage& _storage;
		std::map<std::string, std::shared_ptr<const T>, std::less<>> _map;
		std::mutex _mutex;

		explicit ResourceCache(const Storage& storage)
			: _storage{ storage } {}

		std::shared_ptr<const T> fetch(std::string_view name, const std::function<std::shared_ptr<const T>(std::unique_ptr<Source>&&)>& factory)
		{
			std::scoped_lock lock{ _mutex };
			const auto i = _map.find(name);
			if (i != _map.end())
				return i->second;
			auto source = _storage.open(name);
			if (!source)
				throw ResourceError{ fmt::format("Can't find \"{}\"", name) };
			auto resource_ptr = factory(std::move(source));
			if (!resource_ptr)
				throw DataError{ fmt::format("Can't load \"{}\"", name) }; // We don't have more information at this point. =(
			_map.emplace(name, resource_ptr);
			return resource_ptr;
		}

		auto release_unused()
		{
			std::vector<std::shared_ptr<const T>> unused;
			std::scoped_lock lock{ _mutex };
			size_t released = 0;
			for (auto i = _map.begin(); i != _map.end();)
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
		ResourceLoaderPrivate(const Storage& storage, RenderManager& render_manager)
			: _storage{ storage }, _render_manager{ render_manager } {}

		bool release_unused()
		{
			auto released = _material_cache.release_unused(); // Uses textures.
			released += _mesh_cache.release_unused();
			released += _texture_2d_cache.release_unused();
			return released > 0;
		}

	public:
		const Storage& _storage;
		RenderManager& _render_manager;
		ResourceCache<Material> _material_cache{ _storage };
		ResourceCache<Mesh> _mesh_cache{ _storage };
		ResourceCache<Texture2D> _texture_2d_cache{ _storage };
	};

	ResourceLoader::ResourceLoader(const Storage& storage, RenderManager& render_manager)
		: _private{ std::make_unique<ResourceLoaderPrivate>(storage, render_manager) }
	{
	}

	ResourceLoader::~ResourceLoader() = default;

	std::shared_ptr<const Material> ResourceLoader::load_material(std::string_view name)
	{
		return _private->_material_cache.fetch(name, [this, name](std::unique_ptr<Source>&& source) -> std::shared_ptr<const Material> {
			IonReader ion{ *source };
			// cppcheck-suppress shadowVar
			std::unique_ptr<Source> vertex_shader;
			// cppcheck-suppress shadowVar
			std::unique_ptr<Source> fragment_shader;
			// cppcheck-suppress shadowVar
			std::shared_ptr<const Texture2D> texture;
			// cppcheck-suppress shadowVar
			Texture2D::Filter texture_filter = Texture2D::NearestFilter;
			for (auto token = ion.read(); token.type() != IonToken::Type::End;)
			{
				const auto ion_name = token.to_name();
				if (ion_name == "vertex_shader")
				{
					if (vertex_shader)
						throw DataError{ fmt::format("({}:{}:{}) Duplicate 'vertex_shader'", name, token.line(), token.column()) };
					vertex_shader = _private->_storage.open(ion.read().to_value());
					if (!vertex_shader)
						throw DataError{ fmt::format("({}:{}:{}) Bad 'vertex_shader'", name, token.line(), token.column()) };
					token = ion.read();
				}
				else if (ion_name == "fragment_shader")
				{
					if (fragment_shader)
						throw DataError{ fmt::format("({}:{}:{}) Duplicate 'fragment_shader'", name, token.line(), token.column()) };
					fragment_shader = _private->_storage.open(ion.read().to_value());
					if (!fragment_shader)
						throw DataError{ fmt::format("({}:{}:{}) Bad 'fragment_shader'", name, token.line(), token.column()) };
					token = ion.read();
				}
				else if (ion_name == "texture")
				{
					if (texture)
						throw DataError{ fmt::format("({}:{}:{}) Duplicate 'texture'", name, token.line(), token.column()) };
					const auto texture_name = ion.read().to_value();
					if (texture_name.empty())
						throw DataError{ fmt::format("({}:{}:{}) Bad 'texture'", name, token.line(), token.column()) };
					if (!::read_texture_filter(ion, token.next(ion), texture_filter))
						throw DataError{ fmt::format("({}:{}:{}) Bad texture property", name, token.line(), token.column()) };
					texture = load_texture_2d(texture_name);
				}
				else
					throw DataError{ fmt::format("({}:{}:{}) Unknown material entry '{}'", name, token.line(), token.column(), ion_name) };
			}
			if (!vertex_shader)
				throw DataError{ fmt::format("({}) No 'vertex_shader'", name) };
			if (!fragment_shader)
				throw DataError{ fmt::format("({}) No 'fragment_shader'", name) };
			// cppcheck-suppress shadowVar
			auto program = _private->_render_manager.create_program(vertex_shader->to_string(), fragment_shader->to_string());
			if (!program)
				throw DataError{ fmt::format("({}) Bad 'vertex_shader' or 'fragment_shader'", name) };
			return std::make_shared<MaterialImpl>(std::move(program), std::move(texture), texture_filter);
		});
	}

	std::shared_ptr<const Mesh> ResourceLoader::load_mesh(std::string_view name)
	{
		return _private->_mesh_cache.fetch(name, [this, name](std::unique_ptr<Source>&& source) {
			return _private->_render_manager.load_mesh(*source, name);
		});
	}

	std::shared_ptr<const Texture2D> ResourceLoader::load_texture_2d(std::string_view name)
	{
		return _private->_texture_2d_cache.fetch(name, [this, name](std::unique_ptr<Source>&& source) -> std::shared_ptr<const Texture2D> {
			auto image = Image::load(*source);
			if (!image)
				throw DataError{ fmt::format("Can't load \"{}\"", name) };
			return _private->_render_manager.create_texture_2d(*image);
		});
	}

	void ResourceLoader::release_unused()
	{
		_private->release_unused();
		// cppcheck-suppress assertWithSideEffect
		assert(!_private->release_unused());
	}

	RenderManager& ResourceLoader::render_manager() noexcept
	{
		return _private->_render_manager;
	}
}
