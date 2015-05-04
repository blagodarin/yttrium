#include "renderer.h"

#include <yttrium/matrix.h>
#include "../debug_texture.h"
#include "buffer.h"
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

	std::unique_ptr<IndexBuffer> OpenGlRenderer::create_index_buffer(IndexBuffer::Format format, size_t size, const void* data)
	{
		GLBufferHandle buffer(_gl, GL_ELEMENT_ARRAY_BUFFER_ARB);
		if (!buffer)
			return {};
		const size_t element_size = (format == IndexBuffer::Format::U16) ? 2 : 4;
		const size_t gl_format = (format == IndexBuffer::Format::U16) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
		buffer.bind();
		buffer.initialize(GL_STATIC_DRAW_ARB, size * element_size, data);
		buffer.unbind();
		return std::make_unique<GLIndexBuffer>(format, size, element_size, std::move(buffer), gl_format);
	}

	std::unique_ptr<TextureCache> OpenGlRenderer::create_texture_cache()
	{
		return std::make_unique<GlTextureCache>(*this, _gl);
	}

	std::unique_ptr<VertexBuffer> OpenGlRenderer::create_vertex_buffer(unsigned format, size_t size, const void* data)
	{
		GLBufferHandle buffer(_gl, GL_ARRAY_BUFFER_ARB);
		if (!buffer)
			return {};
		size_t element_size = sizeof(float) * 4; // Geometry data.
		if (format & VertexBuffer::Rgba4F)
			element_size += sizeof(float) * 4;
		if (format & VertexBuffer::Uv2F)
			element_size += sizeof(float) * 2;
		buffer.bind();
		buffer.initialize(GL_STATIC_DRAW_ARB, size * element_size, data);
		buffer.unbind();
		return std::make_unique<GLVertexBuffer>(format, size, element_size, std::move(buffer));
	}

	void OpenGlRenderer::draw_triangles(const VertexBuffer& vertex_buffer, const IndexBuffer& index_buffer)
	{
		const auto& vertices = static_cast<const GLVertexBuffer&>(vertex_buffer);
		const auto& indices = static_cast<const GLIndexBuffer&>(index_buffer);

		_gl.Enable(GL_DEPTH_TEST);
		_gl.DepthFunc(GL_LESS);

		size_t data_offset = 0;
		vertices._buffer.bind();

		if (vertices.format() & VertexBuffer::Rgba4F)
		{
			_gl.EnableClientState(GL_COLOR_ARRAY);
			data_offset += sizeof(float) * 4;
			_gl.ColorPointer(4, GL_FLOAT, vertices.element_size(), reinterpret_cast<void*>(data_offset));
		}

		if (vertices.format() & VertexBuffer::Uv2F)
		{
			_gl.EnableClientState(GL_TEXTURE_COORD_ARRAY);
			data_offset += sizeof(float) * 2;
			_gl.TexCoordPointer(2, GL_FLOAT, vertices.element_size(), reinterpret_cast<void*>(data_offset));
		}
		else
			_gl.Disable(GL_TEXTURE_2D);

		_gl.EnableClientState(GL_VERTEX_ARRAY);
		_gl.VertexPointer(4, GL_FLOAT, vertices.element_size(), 0);

		indices._buffer.bind();
		_gl.DrawElements(GL_TRIANGLES, indices.size(), indices._gl_format, 0);
		indices._buffer.unbind();

		_gl.DisableClientState(GL_VERTEX_ARRAY);
		if (vertices.format() & VertexBuffer::Rgba4F)
			_gl.DisableClientState(GL_COLOR_ARRAY);
		if (vertices.format() & VertexBuffer::Uv2F)
			_gl.DisableClientState(GL_TEXTURE_COORD_ARRAY);
		else
			_gl.Enable(GL_TEXTURE_2D);

		vertices._buffer.unbind();

		_gl.Disable(GL_DEPTH_TEST);
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
