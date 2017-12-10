#include "model.h"

#include <yttrium/math/matrix.h>
#include <yttrium/renderer/context.h>
#include <yttrium/renderer/material.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/resource_loader.h>

Model::Model(ResourceLoader& resource_loader, std::string_view mesh, std::string_view material)
	: _mesh{resource_loader.load_mesh(mesh)}
	, _material{resource_loader.load_material(material)}
{
}

Model::~Model() = default;

void Model::draw(RenderContext& context)
{
	PushMaterial material{context, _material.get()};
	material.set_uniform("u_model", context.model_matrix());
	material.set_uniform("u_mvp", context.full_matrix());
	context.draw_mesh(*_mesh);
}
