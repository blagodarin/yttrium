#include "model.h"

#include <yttrium/exceptions.h>
#include <yttrium/image.h>
#include <yttrium/math/matrix.h>
#include <yttrium/math/vector.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/index_buffer.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/renderer/vertex_buffer.h>
#include <yttrium/storage/reader.h>
#include <yttrium/string.h>

#include <regex>
#include <vector>

namespace
{
	const std::regex _obj_empty_regex(R"(\s*(?:#.*)?)", std::regex::optimize);
	const std::regex _obj_face_regex(R"(\s*f\s+([1-9]\d*)\s+([1-9]\d*)\s+([1-9]\d*)\s*)", std::regex::optimize);
	const std::regex _obj_vertex_regex(R"(\s*v\s+([+-]?\d+\.\d+)\s+([+-]?\d+\.\d+)\s+([+-]?\d+\.\d+)\s*)", std::regex::optimize);

	void load_obj_mesh(const StaticString& path, Renderer& renderer,
		UniquePtr<VertexBuffer>& vertex_buffer, UniquePtr<IndexBuffer>& index_buffer)
	{
		Reader reader(path);
		if (!reader)
			throw DataError("Bad OBJ \"", path, "\"");

		std::vector<Vector4> vertices;
		std::vector<uint16_t> indices;

		std::string line;
		size_t line_number = 0;
		while (reader.read_line(line))
		{
			++line_number;
			if (std::regex_match(line, _obj_empty_regex))
				continue;

			std::smatch match;
			if (std::regex_match(line, match, _obj_vertex_regex))
				vertices.emplace_back(std::stod(match[1]), std::stod(match[2]), std::stod(match[3]));
			else if (std::regex_match(line, match, _obj_face_regex))
			{
				const auto a = std::stoul(match[1]) - 1;
				const auto b = std::stoul(match[2]) - 1;
				const auto c = std::stoul(match[3]) - 1;
				if (a >= vertices.size() || b >= vertices.size() || c >= vertices.size())
					throw DataError("Bad OBJ face (", path, ":", line_number, ")");
				indices.emplace_back(a);
				indices.emplace_back(b);
				indices.emplace_back(c);
			}
			else
				throw DataError("Bad OBJ entry (", path, ":", line_number, ")");
		}

		vertex_buffer = renderer.create_vertex_buffer({ VA::f4 }, vertices.size(), vertices.data());
		index_buffer = renderer.create_index_buffer(IndexFormat::U16, indices.size(), indices.data());
	}
}

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
	::load_obj_mesh("examples/3d/data/cube.obj", _renderer, _vertices, _indices);
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

	_program = _renderer.create_gpu_program(
		Reader("examples/3d/data/chessboard_vs.glsl").to_string(),
		Reader("examples/3d/data/chessboard_fs.glsl").to_string());
}
