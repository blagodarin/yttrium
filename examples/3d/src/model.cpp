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

#include "model.h"

#include <yttrium/math/matrix.h>
#include <yttrium/renderer/material.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/pass.h>
#include <yttrium/resource_loader.h>

Model::Model(ResourceLoader& resource_loader, std::string_view mesh, std::string_view material)
	: _mesh{ resource_loader.load_mesh(mesh) }
	, _material{ resource_loader.load_material(material) }
{
}

Model::~Model() = default;

void Model::draw(RenderPass& pass)
{
	PushMaterial material{ pass, _material.get() };
	material.set_uniform("u_model", pass.model_matrix());
	material.set_uniform("u_mvp", pass.full_matrix());
	pass.draw_mesh(*_mesh);
}
