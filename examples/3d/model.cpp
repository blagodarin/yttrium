#include "model.h"

#include <yttrium/image.h>
#include <yttrium/math/matrix.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/index_buffer.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/renderer/vertex_buffer.h>
#include <yttrium/storage/reader.h>
#include <yttrium/string.h>
#include "obj.h"

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
	_program->set_uniform("u_modelview", _renderer.current_transformation());
	_program->set_uniform("u_projection", _renderer.current_projection());
	_renderer.draw_triangles(*_vertices, *_indices);
}

CubeModel::CubeModel(Renderer& renderer)
	: Model(renderer)
{
	::load_obj_mesh(Reader("examples/3d/data/cube.obj"), _renderer, _vertices, _indices);
	_program = _renderer.create_gpu_program(
		Reader("examples/3d/data/cube_vs.glsl").to_string(),
		Reader("examples/3d/data/cube_fs.glsl").to_string());
}

ChessboardModel::ChessboardModel(Renderer& renderer)
	: Model(renderer)
{
	::load_obj_mesh(Reader("examples/3d/data/chessboard.obj"), _renderer, _vertices, _indices);

	const int size = 128;

	ImageFormat image_format;
	image_format.set_width(size);
	image_format.set_height(size);
	image_format.set_orientation(ImageOrientation::XRightYDown);
	image_format.set_pixel_format(PixelFormat::Bgra, 32);

	std::vector<uint32_t> pixels;
	pixels.reserve(size * size);
	for (int y = size - 1; y >= 0; --y) // OpenGL bottom-to-top row order.
	{
		for (int x = 0; x < size; ++x)
		{
			const bool black = (x ^ y) & 1;
			pixels.emplace_back(black ? 0xff000000 : 0xffdddddd);
		}
	}

	_texture = _renderer.create_texture_2d(image_format, pixels.data(), false);

	_program = _renderer.create_gpu_program(
		Reader("examples/3d/data/chessboard_vs.glsl").to_string(),
		Reader("examples/3d/data/chessboard_fs.glsl").to_string());
}
