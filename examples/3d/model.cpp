#include "model.h"

#include <yttrium/image.h>
#include <yttrium/math/matrix.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/index_buffer.h>
#include <yttrium/renderer/material.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/renderer/vertex_buffer.h>
#include <yttrium/resource_loader.h>
#include <yttrium/storage/reader.h>

Model::Model(Renderer& renderer, ResourceLoader& resource_loader, const StaticString& mesh, const StaticString& material)
	: _renderer(renderer)
	, _mesh(resource_loader.load_mesh(mesh))
	, _material(resource_loader.load_material(material))
{
}

Model::~Model() = default;

void Model::draw()
{
	PushMaterial material(_renderer, _material.get());
	material.set_uniform("u_model", _renderer.model_matrix());
	material.set_uniform("u_mvp", _renderer.full_matrix());
	_renderer.draw_mesh(*_mesh);
}
