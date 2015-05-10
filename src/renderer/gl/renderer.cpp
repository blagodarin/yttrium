#include "renderer.h"

#include <yttrium/matrix.h>
#include <yttrium/utils.h>
#include "../debug_texture.h"
#include "buffer.h"
#include "texture.h"

#include <cassert>

#if Y_IS_DEBUG
	#include <iostream>
#endif

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

	Pointer<Texture2D> OpenGlRenderer::create_texture_2d(const ImageFormat& format, const void* data)
	{
		// NOTE: Keep the new pixel formats in sync with these arrays!

		static const GLenum formats[] =
		{
			GL_LUMINANCE,       // PixelFormat::Gray
			GL_LUMINANCE_ALPHA, // PixelFormat::GrayAlpha
			0,                  // PixelFormat::AlphaGray
			GL_RGB,             // PixelFormat::Rgb
			GL_BGR,             // PixelFormat::Bgr
			GL_RGBA,            // PixelFormat::Rgba
			GL_BGRA,            // PixelFormat::Bgra
			0,                  // PixelFormat::Argb
			0,                  // PixelFormat::Abgr
		};

		static const GLint internal_formats[] =
		{
			GL_LUMINANCE8,        // PixelFormat::Gray
			GL_LUMINANCE8_ALPHA8, // PixelFormat::GrayAlpha
			GL_LUMINANCE8_ALPHA8, // PixelFormat::AlphaGray
			GL_RGB8,              // PixelFormat::Rgb
			GL_RGB8,              // PixelFormat::Bgr
			GL_RGBA8,             // PixelFormat::Rgba
			GL_RGBA8,             // PixelFormat::Bgra
			GL_RGBA8,             // PixelFormat::Argb
			GL_RGBA8,             // PixelFormat::Abgr
		};

		if (format.bits_per_channel() != 8)
			return {};

		const auto data_format = formats[static_cast<size_t>(format.pixel_format())];
		if (!data_format)
			return {};

		const auto internal_format = internal_formats[static_cast<size_t>(format.pixel_format())];

		GLenum target = 0;
		if (_gl.ARB_texture_non_power_of_two || (is_power_of_2(format.width()) && is_power_of_2(format.height())))
			target = GL_TEXTURE_2D;
		else if (_gl.ARB_texture_rectangle)
			target = GL_TEXTURE_RECTANGLE_ARB;
		if (!target)
			return {};

		// NOTE: The following code is not exception-safe:
		// the texture handle won't be released if  an exception is generated.

		GLuint texture = 0;
		_gl.GenTextures(1, &texture); // TODO: Think of using Y_ABORT if this fails.
		if (!texture)
			return {};

		// TODO: Set pixel row alignment if required.

		_gl.BindTexture(target, texture);
		_gl.Hint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		_gl.TexImage2D(target, 0, internal_format, format.width(), format.height(), 0, data_format, GL_UNSIGNED_BYTE, data);

		const auto is_target_enabled = _gl.IsEnabled(target);
		_gl.Enable(target); // ATI bug workaround, see [http://www.opengl.org/wiki/Common_Mistakes#Automatic_mipmap_generation].
		_gl.GenerateMipmap(target);
		if (!is_target_enabled)
			_gl.Disable(target);

		_gl.BindTexture(target, 0);

		return Pointer<Texture2D>(Y_NEW(_allocator, GlTexture2D)(format, _allocator, _gl, target, texture));
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

		vertices._buffer.bind();

		size_t data_offset = sizeof(float) * 4;

		if (vertices.format() & VertexBuffer::Rgba4F)
		{
			_gl.EnableClientState(GL_COLOR_ARRAY);
			_gl.ColorPointer(4, GL_FLOAT, vertices.element_size(), reinterpret_cast<void*>(data_offset));
			data_offset += sizeof(float) * 4;
		}

		if (vertices.format() & VertexBuffer::Uv2F)
		{
			_gl.EnableClientState(GL_TEXTURE_COORD_ARRAY);
			_gl.TexCoordPointer(2, GL_FLOAT, vertices.element_size(), reinterpret_cast<void*>(data_offset));
			data_offset += sizeof(float) * 2;
		}

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
#if Y_IS_DEBUG
		if (_gl.KHR_debug)
		{
			_gl.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			_gl.DebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* opaque)
			{
				static_cast<const OpenGlRenderer*>(opaque)->debug_callback(source, type, id, severity, length, message);
			}, this);
		}
#endif

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

#if Y_IS_DEBUG
	void OpenGlRenderer::debug_callback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar* message) const
	{
		std::cerr << message << std::endl;
	}
#endif
}
