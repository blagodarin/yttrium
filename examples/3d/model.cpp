#include "model.h"

#include <yttrium/image.h>
#include <yttrium/io/reader.h>
#include <yttrium/math/matrix.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/index_buffer.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/renderer/vertex_buffer.h>
#include <yttrium/string.h>

#include <array>
#include <vector>

Model::Model(Renderer& renderer)
	: _renderer(renderer)
{
}

Model::~Model() = default;

void Model::draw(const Vector4& translation)
{
	PushGpuProgram push_gpu_program(_renderer, _program.get());
	PushTexture push_texture(_renderer, _texture.get());
	PushTransformation push_transformation(_renderer, Matrix4::translation(translation));
	_program->set_uniform("u_modelview", _renderer.current_transformation());
	_program->set_uniform("u_projection", _renderer.current_projection());
	_renderer.draw_triangles(*_vertices, *_indices);
}

CubeModel::CubeModel(Renderer& renderer)
	: Model(renderer)
{
	struct Vertex
	{
		Vector4 position;
		Vector4 color;

		Vertex(const Vector4& position, const Vector4& color)
			: position(position)
			, color(color)
		{
		}
	};

	const std::array<Vertex, 8> vertices =
	{
		Vertex({-0.5, -0.5, -0.5}, {0, 0, 0}),
		Vertex({+0.5, -0.5, -0.5}, {0, 0, 1}),
		Vertex({-0.5, +0.5, -0.5}, {0, 1, 0}),
		Vertex({+0.5, +0.5, -0.5}, {0, 1, 1}),
		Vertex({-0.5, -0.5, +0.5}, {1, 0, 0}),
		Vertex({+0.5, -0.5, +0.5}, {1, 0, 1}),
		Vertex({-0.5, +0.5, +0.5}, {1, 1, 0}),
		Vertex({+0.5, +0.5, +0.5}, {1, 1, 1}),
	};

	_vertices = _renderer.create_vertex_buffer({VA::f4, VA::f4}, vertices.size(), vertices.data());

	const std::array<uint16_t, 36> indices =
	{
		// Bottom.
		0, 2, 1,
		1, 2, 3,

		// Front.
		0, 1, 4,
		4, 1, 5,

		// Top.
		4, 5, 6,
		6, 5, 7,

		// Back.
		2, 6, 3,
		3, 6, 7,

		// Right.
		5, 1, 7,
		7, 1, 3,

		// Left.
		2, 0, 6,
		6, 0, 4,
	};

	_indices = _renderer.create_index_buffer(IndexFormat::U16, indices.size(), indices.data());

	_program = _renderer.create_gpu_program(
		Reader("examples/3d/data/cube_vs.glsl").to_string(),
		Reader("examples/3d/data/cube_fs.glsl").to_string());
}

ChessboardModel::ChessboardModel(Renderer& renderer)
	: Model(renderer)
{
	const int size = 128;

	struct Vertex
	{
		float x, y, z, w;
		float u, v;
		float nx, ny, nz, nw;

		Vertex(float x, float y, float u, float v)
			: x(x), y(y), z(0), w(1)
			, u(u), v(v)
			, nx(0), ny(0), nz(1), nw(1)
		{
		}
	};

	const std::array<Vertex, 4> vertices =
	{
		Vertex(-size / 2, -size / 2, 0, 0),
		Vertex( size / 2, -size / 2, 1, 0),
		Vertex(-size / 2,  size / 2, 0, 1),
		Vertex( size / 2,  size / 2, 1, 1),
	};

	_vertices = _renderer.create_vertex_buffer({VA::f4, VA::f2, VA::f4}, vertices.size(), vertices.data());

	const std::array<uint16_t, 6> indices =
	{
		0, 1, 2,
		2, 1, 3,
	};

	_indices = _renderer.create_index_buffer(IndexFormat::U16, indices.size(), indices.data());

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
	_texture->set_filter(Texture2D::NearestFilter | Texture2D::AnisotropicFilter);

	_program = _renderer.create_gpu_program(
		Reader("examples/3d/data/chessboard_vs.glsl").to_string(),
		Reader("examples/3d/data/chessboard_fs.glsl").to_string());
}
