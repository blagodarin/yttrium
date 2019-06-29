//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <memory>
#include <string_view>

namespace Yttrium
{
	class Material;
	class Mesh;
	class RenderPass;
	class ResourceLoader;
}

using namespace Yttrium;

class Model
{
public:
	Model(ResourceLoader&, std::string_view mesh, std::string_view material);
	~Model();

	void draw(RenderPass&);

private:
	const std::shared_ptr<const Mesh> _mesh;
	const std::shared_ptr<const Material> _material;
};
