#include "model.h"

#include <yttrium/math/matrix.h>
#include <yttrium/renderer/material.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/pass.h>
#include <yttrium/resource_loader.h>

Model::Model(ResourceLoader& resource_loader, std::string_view mesh, std::string_view material)
	: _mesh{resource_loader.load_mesh(mesh)}
	, _material{resource_loader.load_material(material)}
{
}

Model::~Model() = default;

void Model::draw(RenderPass& pass)
{
	PushMaterial material{pass, _material.get()};
	material.set_uniform("u_model", pass.model_matrix());
	material.set_uniform("u_mvp", pass.full_matrix());
	pass.draw_mesh(*_mesh);
}
