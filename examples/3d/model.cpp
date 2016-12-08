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
	const auto transformation = Matrix4::translation(translation);
	PushTransformation push_transformation(_renderer, transformation);
	_program->set_uniform("u_model", transformation);
	_program->set_uniform("u_modelview", _renderer.current_transformation());
	_program->set_uniform("u_projection", _renderer.current_projection());
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
	_mesh = resource_loader.load_mesh("examples/3d/data/chessboard.obj");

	const int size = 128;

	ImageFormat format;
	format.set_width(size);
	format.set_height(size);
	format.set_pixel_format(PixelFormat::Bgra, 32);
	format.set_orientation(ImageOrientation::XRightYDown);

	Image image(format);
	for (int y = 0; y < size; ++y)
	{
		for (int x = 0; x < size; ++x)
		{
			const auto pixel = static_cast<uint8_t*>(image.data()) + y * format.row_size() + x * 4;
			const auto color = (x ^ y) & 1 ? uint8_t{ 0xdd } : uint8_t{ 0x00 };
			pixel[0] = color;
			pixel[1] = color;
			pixel[2] = color;
			pixel[3] = 0xff;
		}
	}

	_texture = _renderer.create_texture_2d(image);

	_program = _renderer.create_gpu_program(
		Reader("examples/3d/data/chessboard_vs.glsl").to_string(),
		Reader("examples/3d/data/chessboard_fs.glsl").to_string());
}
