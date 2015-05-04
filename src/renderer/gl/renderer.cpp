#include "renderer.h"

#include <yttrium/matrix.h>
#include "../debug_texture.h"
#include "index_buffer.h"
#include "texture_cache.h"

#include <cassert>

namespace Yttrium
{
	OpenGlRenderer::OpenGlRenderer(WindowBackend& window, Allocator* allocator)
		: RendererImpl(window, allocator)
	{
		_gl.initialize(window);
	}

	OpenGlRenderer::~OpenGlRenderer()
	{
		_gl.DeleteTextures(1, &_debug_texture);
	}

	std::unique_ptr<IndexBuffer> OpenGlRenderer::create_index_buffer(IndexBuffer::Format format, size_t size)
	{
		GLuint buffer = 0;
		_gl.GenTextures(1, &buffer); // TODO: Think of using Y_ABORT if this fails.
		if (buffer == 0)
			return {};
		_gl.BindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, buffer);
		_gl.BufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, size * (format == IndexBuffer::Format::U16 ? 2 : 4), nullptr, GL_STATIC_DRAW_ARB);
		// NOTE: If the next line throws, nothing will release the allocated buffer.
		return std::make_unique<GlIndexBuffer>(format, size, _gl, buffer);
	}

	std::unique_ptr<TextureCache> OpenGlRenderer::create_texture_cache()
	{
		return std::make_unique<GlTextureCache>(*this, _gl);
	}

	void OpenGlRenderer::draw_cube(const Vector4& center, float size)
	{
		assert(!_debug_rendering);

		struct Vertex3D
		{
			Vector4 position;
			Vector4 color;

			Vertex3D(const Vector4& position, const Vector4& color)
				: position(position)
				, color(color)
			{
			}
		};

		const auto radius = size / 2;

		const std::array<Vertex3D, 8> vertices =
		{
			Vertex3D(Vector4(center.x - radius, center.y - radius, center.z - radius), Vector4(0, 0, 0)),
			Vertex3D(Vector4(center.x + radius, center.y - radius, center.z - radius), Vector4(0, 0, 1)),
			Vertex3D(Vector4(center.x - radius, center.y + radius, center.z - radius), Vector4(0, 1, 0)),
			Vertex3D(Vector4(center.x + radius, center.y + radius, center.z - radius), Vector4(0, 1, 1)),
			Vertex3D(Vector4(center.x - radius, center.y - radius, center.z + radius), Vector4(1, 0, 0)),
			Vertex3D(Vector4(center.x + radius, center.y - radius, center.z + radius), Vector4(1, 0, 1)),
			Vertex3D(Vector4(center.x - radius, center.y + radius, center.z + radius), Vector4(1, 1, 0)),
			Vertex3D(Vector4(center.x + radius, center.y + radius, center.z + radius), Vector4(1, 1, 1)),
		};

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

		_gl.Disable(GL_TEXTURE_2D);
		_gl.Enable(GL_DEPTH_TEST);
		_gl.DepthFunc(GL_LESS);

		_gl.EnableClientState(GL_COLOR_ARRAY);
		_gl.ColorPointer(4, GL_FLOAT, sizeof(Vertex3D), &vertices[0].color);

		_gl.EnableClientState(GL_VERTEX_ARRAY);
		_gl.VertexPointer(4, GL_FLOAT, sizeof(Vertex3D), &vertices[0].position);

		_gl.DrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);

		_gl.DisableClientState(GL_VERTEX_ARRAY);
		_gl.DisableClientState(GL_COLOR_ARRAY);

		_gl.Disable(GL_DEPTH_TEST);
		_gl.Enable(GL_TEXTURE_2D);
	}

	void OpenGlRenderer::clear()
	{
		_gl.ClearColor(0.5, 0.5, 0.5, 0);
		_gl.ClearDepth(1);
		_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGlRenderer::take_screenshot(Image& image)
	{
		GLint unpack_alignment;
		_gl.GetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);
		image.set_size(_window_size.width, _window_size.height, unpack_alignment);

		GLint read_buffer;
		_gl.GetIntegerv(GL_READ_BUFFER, &read_buffer);
		_gl.ReadBuffer(GL_FRONT);
		_gl.ReadPixels(0, 0, _window_size.width, _window_size.height, GL_RGB, GL_UNSIGNED_BYTE, image.data());
		_gl.ReadBuffer(read_buffer);
	}

	bool OpenGlRenderer::initialize()
	{
		if (!check_min_version(1, 2))
			return false; // TODO: Report error.

		_gl.Enable(GL_TEXTURE_2D);
		_gl.Enable(GL_BLEND);
		_gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		_gl.Hint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		_gl.GenTextures(1, &_debug_texture);
		if (!_debug_texture)
			return false; // TODO: Report error.
		_gl.BindTexture(GL_TEXTURE_2D, _debug_texture);
		_gl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, DebugTexture::width, DebugTexture::height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, DebugTexture::data);
		_gl.BindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	void OpenGlRenderer::flush_2d_impl()
	{
		assert(!_vertices_2d.empty());

		_gl.EnableClientState(GL_COLOR_ARRAY);
		_gl.ColorPointer(4, GL_FLOAT, sizeof(Vertex2D), &_vertices_2d[0].color);

		if (_debug_rendering || _texture)
		{
			_gl.EnableClientState(GL_TEXTURE_COORD_ARRAY);
			_gl.TexCoordPointer(2, GL_FLOAT, sizeof(Vertex2D), &_vertices_2d[0].texture);
		}

		_gl.EnableClientState(GL_VERTEX_ARRAY);
		_gl.VertexPointer(2, GL_FLOAT, sizeof(Vertex2D), &_vertices_2d[0].position);

		_gl.DrawElements(GL_TRIANGLE_STRIP, _indices_2d.size(), GL_UNSIGNED_SHORT, &_indices_2d[0]);

		_gl.DisableClientState(GL_VERTEX_ARRAY);
		_gl.DisableClientState(GL_COLOR_ARRAY);
		_gl.DisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	void OpenGlRenderer::set_debug_texture_impl()
	{
		_gl.BindTexture(GL_TEXTURE_2D, _debug_texture);
		_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		_gl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	void OpenGlRenderer::set_projection(const Matrix4& matrix)
	{
		_gl.MatrixMode(GL_PROJECTION);
		_gl.LoadMatrixf(matrix.data());
		_gl.MatrixMode(GL_MODELVIEW);
		_gl.LoadMatrixf(Matrix4().data());
	}

	void OpenGlRenderer::set_transformation(const Matrix4& matrix)
	{
		_gl.MatrixMode(GL_MODELVIEW);
		_gl.LoadMatrixf(matrix.data());
	}

	void OpenGlRenderer::update_window_size()
	{
		_gl.Viewport(0, 0, _window_size.width, _window_size.height);
	}

	bool OpenGlRenderer::check_min_version(int major, int minor)
	{
		// NOTE: I believe that the version number is in form "<one digit>.<one digit><anything>".
		const int actual_major = _gl.VERSION[0] - '0';
		const int actual_minor = _gl.VERSION[2] - '0';
		return actual_major > major || (actual_major == major && actual_minor >= minor);
	}
}
