#include "model.h"

#include <yttrium/image.h>
#include <yttrium/math/matrix.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/index_buffer.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/renderer/vertex_buffer.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/storage/reader.h>
#include <yttrium/string.h>

Model::Model(Renderer& renderer)
	: _renderer(renderer)
{
}

Model::~Model() = default;

void Model::draw(const Vector4& translation)
{
	PushGpuProgram push_gpu_program(_renderer, _program.get());
	PushTexture push_texture(_renderer, _texture.get(), static_cast<Texture2D::Filter>(Texture2D::NearestFilter | Texture2D::AnisotropicFilter));
	PushTransformation push_transformation(_renderer, Matrix4::translation(translation));
	_program->set_uniform("u_model", _renderer.model_matrix());
	_program->set_uniform("u_mvp", _renderer.full_matrix());
	_renderer.draw_mesh(*_mesh);
}

CubeModel::CubeModel(Renderer& renderer, ResourceLoader& resource_loader)
	: Model(renderer)
{
	_mesh = resource_loader.load_mesh("examples/3d/data/cube.obj");
	_program = _renderer.create_gpu_program(
		Reader("examples/3d/data/cube_vs.glsl").to_string(),
		Reader("examples/3d/data/cube_fs.glsl").to_string());
}

ChessboardModel::ChessboardModel(Renderer& renderer, ResourceLoader& resource_loader)
	: Model(renderer)
{
	_mesh = resource_loader.load_mesh("examples/3d/data/checkerboard.obj");
	_texture = resource_loader.load_texture_2d("examples/3d/data/checkerboard.tga");
	_program = _renderer.create_gpu_program(
		Reader("examples/3d/data/checkerboard_vs.glsl").to_string(),
		Reader("examples/3d/data/checkerboard_fs.glsl").to_string());
}
