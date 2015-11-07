#include "renderer.h"

#include <yttrium/log.h>
#include <yttrium/matrix.h>
#include <yttrium/utils.h>
#include "buffer.h"
#include "gpu_program.h"
#include "texture.h"

#include <cassert>

#if Y_IS_DEBUG
	#include <iostream>
#endif

namespace Yttrium
{
	GLRenderer::GLRenderer(WindowBackend& window, Allocator& allocator)
		: RendererImpl(allocator)
	{
		_gl.initialize(window);
	}

	Pointer<GpuProgram> GLRenderer::create_gpu_program(const StaticString& vertex_shader, const StaticString& fragment_shader)
	{
		GlShaderHandle vertex(_gl, GL_VERTEX_SHADER);
		if (!vertex.compile(vertex_shader))
		{
			Log() << vertex.info_log(allocator());
			return {};
		}

		GlShaderHandle fragment(_gl, GL_FRAGMENT_SHADER);
		if (!fragment.compile(fragment_shader))
		{
			Log() << fragment.info_log(allocator());
			return {};
		}

		auto result = make_pointer<GlGpuProgram>(allocator(), *this, std::move(vertex), std::move(fragment), _gl);
		if (!result->link())
			return {};

		return std::move(result);
	}

	Pointer<IndexBuffer> GLRenderer::create_index_buffer(IndexBuffer::Format format, size_t size, const void* data)
	{
		const size_t element_size = (format == IndexBuffer::Format::U16) ? 2 : 4;
		const size_t gl_format = (format == IndexBuffer::Format::U16) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

		GlBufferHandle buffer(_gl, GL_ELEMENT_ARRAY_BUFFER_ARB);
		buffer.bind();
		buffer.initialize(GL_STATIC_DRAW_ARB, size * element_size, data);
		buffer.unbind();
		return make_pointer<GlIndexBuffer>(allocator(), format, size, element_size, std::move(buffer), gl_format);
	}

	SharedPtr<Texture2D> GLRenderer::create_texture_2d(const ImageFormat& format, const void* data, bool no_mipmaps)
	{
		if (format.bits_per_channel() != 8)
			return {};

		GLenum internal_format = 0;
		GLenum data_format = 0;
		GLenum data_type = 0;
		switch (format.pixel_format())
		{
		case PixelFormat::Gray:
			internal_format = GL_LUMINANCE8;
			data_format = GL_LUMINANCE;
			data_type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::GrayAlpha:
			internal_format = GL_LUMINANCE8_ALPHA8;
			data_format = GL_LUMINANCE_ALPHA;
			data_type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::Rgb:
			internal_format = GL_RGB8;
			data_format = GL_RGB;
			data_type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::Bgr:
			internal_format = GL_RGB8;
			data_format = GL_BGR;
			data_type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::Rgba:
			internal_format = GL_RGBA8;
			data_format = GL_RGBA;
			data_type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::Bgra:
			internal_format = GL_RGBA8;
			data_format = GL_BGRA;
			data_type = GL_UNSIGNED_BYTE;
			break;
		default:
			return {};
		}

		GlTextureHandle texture(_gl, GL_TEXTURE_2D);
		texture.bind();
		assert(is_power_of_2(format.row_alignment()) && format.row_alignment() <= 8); // OpenGL requirements.
		_gl.PixelStorei(GL_PACK_ALIGNMENT, format.row_alignment());
		_gl.TexImage2D(GL_TEXTURE_2D, 0, internal_format, format.width(), format.height(), 0, data_format, data_type, data);
		if (!no_mipmaps)
		{
			const auto is_target_enabled = _gl.IsEnabled(GL_TEXTURE_2D);
			_gl.Enable(GL_TEXTURE_2D); // ATI bug workaround, see [http://www.opengl.org/wiki/Common_Mistakes#Automatic_mipmap_generation].
			_gl.GenerateMipmap(GL_TEXTURE_2D);
			if (!is_target_enabled)
				_gl.Disable(GL_TEXTURE_2D);
		}
		texture.unbind();

		return SharedPtr<Texture2D>(Y_NEW(&allocator(), GlTexture2D)(*this, format, !no_mipmaps, std::move(texture)));
	}

	Pointer<VertexBuffer> GLRenderer::create_vertex_buffer(unsigned format, size_t size, const void* data)
	{
		size_t element_size = sizeof(float) * 4; // Geometry data.
		if (format & VertexBuffer::Rgba4F)
			element_size += sizeof(float) * 4;
		if (format & VertexBuffer::Uv2F)
			element_size += sizeof(float) * 2;
		if (format & VertexBuffer::Normal4F)
			element_size += sizeof(float) * 4;

		GlBufferHandle buffer(_gl, GL_ARRAY_BUFFER_ARB);
		buffer.bind();
		buffer.initialize(GL_STATIC_DRAW_ARB, size * element_size, data);
		buffer.unbind();
		return make_pointer<GlVertexBuffer>(allocator(), format, size, element_size, std::move(buffer));
	}

	void GLRenderer::draw_triangles(const VertexBuffer& vertex_buffer, const IndexBuffer& index_buffer)
	{
		update_state();

		const auto& vertices = static_cast<const GlVertexBuffer&>(vertex_buffer);
		const auto& indices = static_cast<const GlIndexBuffer&>(index_buffer);

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

		if (vertices.format() & VertexBuffer::Normal4F)
		{
			_gl.EnableClientState(GL_NORMAL_ARRAY);
			_gl.NormalPointer(GL_FLOAT, vertices.element_size(), reinterpret_cast<void*>(data_offset));
			data_offset += sizeof(float) * 4;
		}

		_gl.EnableClientState(GL_VERTEX_ARRAY);
		_gl.VertexPointer(4, GL_FLOAT, vertices.element_size(), 0);

		indices._buffer.bind();

		_gl.DrawElements(GL_TRIANGLES, indices.size(), indices._gl_format, 0);
		++_statistics._draw_calls;
		_statistics._triangles += indices.size() / 3;

		indices._buffer.unbind();

		_gl.DisableClientState(GL_VERTEX_ARRAY);
		if (vertices.format() & VertexBuffer::Rgba4F)
			_gl.DisableClientState(GL_COLOR_ARRAY);
		if (vertices.format() & VertexBuffer::Uv2F)
			_gl.DisableClientState(GL_TEXTURE_COORD_ARRAY);
		if (vertices.format() & VertexBuffer::Normal4F)
			_gl.DisableClientState(GL_NORMAL_ARRAY);

		vertices._buffer.unbind();

		_gl.Disable(GL_DEPTH_TEST);
	}

	void GLRenderer::clear()
	{
		_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void GLRenderer::take_screenshot(Image& image)
	{
		const auto& size = window_size();

		GLint unpack_alignment;
		_gl.GetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);
		image.set_size(size.width, size.height, unpack_alignment);

		GLint read_buffer;
		_gl.GetIntegerv(GL_READ_BUFFER, &read_buffer);
		_gl.ReadBuffer(GL_FRONT);
		_gl.ReadPixels(0, 0, size.width, size.height, GL_RGB, GL_UNSIGNED_BYTE, image.data());
		_gl.ReadBuffer(read_buffer);
	}

	bool GLRenderer::initialize()
	{
#if Y_IS_DEBUG
		if (_gl.KHR_debug)
		{
			_gl.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			_gl.DebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* opaque)
			{
				static_cast<const GLRenderer*>(opaque)->debug_callback(source, type, id, severity, length, message);
			}, this);
		}
#endif

		if (!check_min_version(2, 0))
			return false;

		_gl.Enable(GL_CULL_FACE); // The default behavior is to cull back (clockwise) faces.
		_gl.Enable(GL_TEXTURE_2D);
		_gl.Enable(GL_BLEND);
		_gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		_gl.Hint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		_gl.Hint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		_gl.ClearColor(0.5, 0.5, 0.5, 0);
		_gl.ClearDepth(1);

		return true;
	}

	void GLRenderer::flush_2d_impl(const StdVector<Vertex2D>& vertices, const StdVector<uint16_t>& indices)
	{
		update_state();

		_gl.EnableClientState(GL_COLOR_ARRAY);
		_gl.ColorPointer(4, GL_FLOAT, sizeof(Vertex2D), &vertices[0].color);

		if (current_texture_2d())
		{
			_gl.EnableClientState(GL_TEXTURE_COORD_ARRAY);
			_gl.TexCoordPointer(2, GL_FLOAT, sizeof(Vertex2D), &vertices[0].texture);
		}

		_gl.EnableClientState(GL_VERTEX_ARRAY);
		_gl.VertexPointer(2, GL_FLOAT, sizeof(Vertex2D), &vertices[0].position);

		_gl.DrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_SHORT, indices.data());
		++_statistics._draw_calls;
		_statistics._triangles += indices.size() - 2;

		_gl.DisableClientState(GL_VERTEX_ARRAY);
		_gl.DisableClientState(GL_COLOR_ARRAY);
		_gl.DisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	void GLRenderer::set_program(const GpuProgram* program)
	{
		_gl.UseProgram(program ? static_cast<const GlGpuProgram*>(program)->handle() : 0);
	}

	void GLRenderer::set_projection(const Matrix4& matrix)
	{
		_gl.MatrixMode(GL_PROJECTION);
		_gl.LoadMatrixf(matrix.data());
	}

	void GLRenderer::set_texture(const Texture2D* texture)
	{
		if (!texture)
		{
			_gl.BindTexture(GL_TEXTURE_2D, 0);
			return;
		}
		static_cast<const GlTexture2D*>(texture)->bind();
	}

	void GLRenderer::set_transformation(const Matrix4& matrix)
	{
		_gl.MatrixMode(GL_MODELVIEW);
		_gl.LoadMatrixf(matrix.data());
	}

	void GLRenderer::set_window_size_impl(const Size& size)
	{
		_gl.Viewport(0, 0, size.width, size.height);
	}

	bool GLRenderer::check_min_version(int major, int minor)
	{
		// NOTE: I believe that the version number is in form "<one digit>.<one digit><anything>".
		const int actual_major = _gl.VERSION[0] - '0';
		const int actual_minor = _gl.VERSION[2] - '0';
		if (actual_major > major || (actual_major == major && actual_minor >= minor))
			return true;
		Log() << "OpenGL "_s << dec(major) << "."_s << dec(minor) << " is required, but only OpenGL "_s
			<< dec(actual_major) << "."_s << dec(actual_minor) << " is supported"_s;
		return false;
	}

#if Y_IS_DEBUG
	void GLRenderer::debug_callback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar* message) const
	{
		std::cerr << message << std::endl;
	}
#endif
}
