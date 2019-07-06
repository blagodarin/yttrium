//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <yttrium/api.h>

#include <memory>
#include <string_view>

namespace Yttrium
{
	class Material;
	class Mesh;
	class RenderManager;
	class Source;
	class Storage;
	class Texture2D;
	class Translation;

	///
	class Y_ENGINE_API ResourceLoader
	{
	public:
		///
		ResourceLoader(const Storage&, RenderManager* = nullptr);

		///
		~ResourceLoader();

		/// Loads a resource by name (if it isn't already loaded) and returns a pointer to the loaded resource.
		/// Throws ResourceError if there is no such resource.
		/// Throws DataError if the resource can't be loaded.
		std::shared_ptr<const Material> load_material(std::string_view name);
		std::shared_ptr<const Mesh> load_mesh(std::string_view name);
		std::shared_ptr<const Texture2D> load_texture_2d(std::string_view name);
		std::shared_ptr<const Translation> load_translation(std::string_view name);

		///
		std::unique_ptr<Source> open(std::string_view name);

		/// Releases all loaded resources that have no references outside of the loader.
		void release_unused();

		///
		RenderManager* render_manager() const;

	private:
		const std::unique_ptr<class ResourceLoaderPrivate> _private;
	};
}
