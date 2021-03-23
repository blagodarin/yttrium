// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string_view>

namespace Yt
{
	class Material;
	class Mesh;
	class RenderManager;
	class Storage;
	class Texture2D;

	class ResourceLoader
	{
	public:
		ResourceLoader(const Storage&, RenderManager&);
		~ResourceLoader();

		/// Loads a resource by name (if it isn't already loaded) and returns a pointer to the loaded resource.
		/// Throws ResourceError if there is no such resource.
		/// Throws DataError if the resource can't be loaded.
		std::shared_ptr<const Material> load_material(std::string_view name);
		std::shared_ptr<const Mesh> load_mesh(std::string_view name);
		std::shared_ptr<const Texture2D> load_texture_2d(std::string_view name);

		/// Releases all loaded resources that have no references outside of the loader.
		void release_unused();

		///
		RenderManager& render_manager() noexcept;

	private:
		const std::unique_ptr<class ResourceLoaderPrivate> _private;
	};
}
