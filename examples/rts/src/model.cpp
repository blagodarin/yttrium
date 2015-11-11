#include "model.h"

#include <yttrium/image.h>
#include <yttrium/matrix.h>
#include <yttrium/vertex_buffer.h>

#include <array>
#include <vector>

Model::Model(Renderer& renderer)
	: _renderer(renderer)
{
}

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

	_indices = _renderer.create_index_buffer(IndexBuffer::Format::U16, indices.size(), indices.data());

	_program = _renderer.create_gpu_program(
		"#version 150\n"
		"#extension GL_ARB_explicit_attrib_location : enable\n" // TODO: Remove at #version 330.
		"\n"
		"layout(location = 0) in vec4 in_position;\n"
		"layout(location = 1) in vec4 in_color;\n"
		"\n"
		"uniform mat4 u_modelview;\n"
		"uniform mat4 u_projection;\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
			"gl_Position = u_projection * u_modelview * in_position;\n"
			"color = in_color;\n"
		"}\n",

		"#version 150\n"
		"\n"
		"in vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
			"gl_FragColor = color;\n"
		"}\n"
	);
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

	_indices = _renderer.create_index_buffer(IndexBuffer::Format::U16, indices.size(), indices.data());

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
		"#version 150\n"
		"#extension GL_ARB_explicit_attrib_location : enable\n" // TODO: Remove at #version 330.
		"\n"
		"layout(location = 0) in vec4 in_position;\n"
		"layout(location = 1) in vec2 in_texcoord;\n"
		"layout(location = 2) in vec4 in_normal;\n"
		"\n"
		"uniform mat4 u_modelview;\n"
		"uniform mat4 u_projection;\n"
		"\n"
		"out vec2 texcoord;\n"
		"out vec3 view_direction;\n"
		"out vec3 n;\n"
		"\n"
		"void main()\n"
		"{\n"
			"view_direction = vec3(u_modelview * in_position);\n"
			"n = normalize(transpose(inverse(mat3(u_modelview))) * vec3(in_normal));\n"
			"gl_Position = u_projection * u_modelview * in_position;\n"
			"texcoord = in_texcoord;\n"
		"}\n",

		"#version 150\n"
		"\n"
		"in vec2 texcoord;\n"
		"in vec3 view_direction;\n"
		"in vec3 n;\n"
		"\n"
		"uniform sampler2D surface_texture;\n"
		"\n"
		"void main()\n"
		"{"
			"vec4 c = texture2D(surface_texture, texcoord.xy);\n"
			"if (view_direction.x < 0.0)\n"
			"{\n"
				"float gray = dot(c.xyz, vec3(1.0, 1.0, 1.0));\n"
				"gl_FragColor = vec4(gray, gray, gray, c.a * 0.25);\n"
			"}\n"
			"else\n"
			"{\n"
				"vec3 l = normalize(-view_direction);\n"
				"vec3 e = normalize(-view_direction);\n"
				"vec3 r = normalize(-reflect(l, n));\n"
				"vec3 diffuse = c.rgb * clamp(dot(n, l), 0.0, 1.0);\n"
				"vec3 specular = vec3(1.0, 1.0, 1.0) * clamp(dot(r, e), 0.0, 1.0);\n"
				"gl_FragColor = vec4(diffuse + specular, c.a);\n"
			"}\n"
		"}\n"
	);
}
