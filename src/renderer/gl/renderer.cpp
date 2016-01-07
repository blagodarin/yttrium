#include "renderer.h"

#include <yttrium/log.h>
#include <yttrium/math/matrix.h>
#include <yttrium/utils.h>
#include "buffer.h"
#include "gpu_program.h"
#include "texture.h"

#include <cassert>

#if Y_IS_DEBUG
	#include <csignal>
	#include <iostream>
#endif

namespace Yttrium
{
	GlRenderer::GlRenderer(Allocator& allocator)
		: RendererImpl(allocator)
		, _gl(allocator)
		, _2d_ibo(_gl, GL_ELEMENT_ARRAY_BUFFER)
		, _2d_vbo(_gl, GL_ARRAY_BUFFER)
		, _2d_vao(_gl)
	{
#if Y_IS_DEBUG
		if (_gl.KHR_debug)
		{
			_gl.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			_gl.DebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* opaque)
			{
				static_cast<const GlRenderer*>(opaque)->debug_callback(source, type, id, severity, length, message);
			}, this);
		}
#endif
		_gl.Enable(GL_CULL_FACE); // The default behavior is to cull back (clockwise) faces.
		_gl.Enable(GL_BLEND);
		_gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		_gl.ClearColor(0.5, 0.5, 0.5, 0);
		_gl.ClearDepth(1);

		_2d_vbo.initialize(GL_DYNAMIC_DRAW, 0, nullptr); // The next line fails without this.
		_2d_vao.bind_vertex_buffer(0, _2d_vbo.get(), 0, sizeof(Vertex2D));
		_2d_vao.vertex_attrib_binding(0, 0);
		_2d_vao.vertex_attrib_format(0, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, position));
		_2d_vao.vertex_attrib_binding(1, 0);
		_2d_vao.vertex_attrib_format(1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, color));
		_2d_vao.vertex_attrib_binding(2, 0);
		_2d_vao.vertex_attrib_format(2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, texture));
	}

	Pointer<GpuProgram> GlRenderer::create_gpu_program(const StaticString& vertex_shader, const StaticString& fragment_shader)
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

	Pointer<IndexBuffer> GlRenderer::create_index_buffer(IndexBuffer::Format format, size_t count, const void* data)
	{
		const size_t element_size = (format == IndexBuffer::Format::U16) ? 2 : 4;
		const size_t gl_format = (format == IndexBuffer::Format::U16) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

		GlBufferHandle buffer(_gl, GL_ELEMENT_ARRAY_BUFFER);
		buffer.initialize(GL_STATIC_DRAW, count * element_size, data);
		return make_pointer<GlIndexBuffer>(allocator(), format, count, element_size, std::move(buffer), gl_format);
	}

	SharedPtr<Texture2D> GlRenderer::create_texture_2d(const ImageFormat& format, const void* data, bool no_mipmaps)
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
		// TODO: Remove deprecated LUMINANCE formats.

		GlTextureHandle texture(_gl, GL_TEXTURE_2D);
		assert(is_power_of_2(format.row_alignment()) && format.row_alignment() <= 8); // OpenGL requirements.
		_gl.PixelStorei(GL_PACK_ALIGNMENT, format.row_alignment());
		texture.set_data(0, internal_format, format.width(), format.height(), data_format, data_type, data);
		if (!no_mipmaps)
			texture.generate_mipmaps();
		return SharedPtr<Texture2D>(Y_NEW(&allocator(), GlTexture2D)(*this, format, !no_mipmaps, std::move(texture)));
	}

	Pointer<VertexBuffer> GlRenderer::create_vertex_buffer(std::initializer_list<VA> format, size_t count, const void* data)
	{
		assert(format.size() > 0);

		GlVertexArrayHandle vertex_array(_gl);

		GLuint index = 0;
		size_t offset = 0;
		for (const auto type : format)
		{
			vertex_array.vertex_attrib_binding(index, 0);
			switch (type)
			{
			case VA::f:
				vertex_array.vertex_attrib_format(index, 1, GL_FLOAT, GL_FALSE, offset);
				offset += sizeof(float);
				break;
			case VA::f2:
				vertex_array.vertex_attrib_format(index, 2, GL_FLOAT, GL_FALSE, offset);
				offset += sizeof(float) * 2;
				break;
			case VA::f3:
				vertex_array.vertex_attrib_format(index, 3, GL_FLOAT, GL_FALSE, offset);
				offset += sizeof(float) * 3;
				break;
			case VA::f4:
				vertex_array.vertex_attrib_format(index, 4, GL_FLOAT, GL_FALSE, offset);
				offset += sizeof(float) * 4;
				break;
			}
			++index;
		}

		GlBufferHandle buffer(_gl, GL_ARRAY_BUFFER);
		buffer.initialize(GL_STATIC_DRAW, count * offset, data);
		vertex_array.bind_vertex_buffer(0, buffer.get(), 0, offset);

		return make_pointer<GlVertexBuffer>(allocator(), count, offset, std::move(buffer), std::move(vertex_array));
	}

	void GlRenderer::draw_triangles(const VertexBuffer& vertex_buffer, const IndexBuffer& index_buffer)
	{
		update_state();

		const auto& vertices = static_cast<const GlVertexBuffer&>(vertex_buffer);
		const auto& indices = static_cast<const GlIndexBuffer&>(index_buffer);

		_gl.Enable(GL_DEPTH_TEST);
		_gl.DepthFunc(GL_LESS);

		vertices._vertex_array.bind();
		indices._buffer.bind();
		_gl.DrawElements(GL_TRIANGLES, indices.size(), indices._gl_format, 0);
		indices._buffer.unbind();
		vertices._vertex_array.unbind();

		++_statistics._draw_calls;
		_statistics._triangles += indices.size() / 3;

		_gl.Disable(GL_DEPTH_TEST);
	}

	void GlRenderer::clear()
	{
		_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void GlRenderer::take_screenshot(Image& image)
	{
		const auto& size = window_size();

		GLint unpack_alignment;
		_gl.GetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_alignment);
		image.set_size(size.width(), size.height(), unpack_alignment);

		GLint read_buffer;
		_gl.GetIntegerv(GL_READ_BUFFER, &read_buffer);
		_gl.ReadBuffer(GL_FRONT);
		_gl.ReadPixels(0, 0, size.width(), size.height(), GL_RGB, GL_UNSIGNED_BYTE, image.data());
		_gl.ReadBuffer(read_buffer);
	}

	void GlRenderer::flush_2d_impl(const Buffer& vertices, const Buffer& indices)
	{
		update_state();

		if (vertices.size() > _2d_vbo.size())
			_2d_vbo.initialize(GL_DYNAMIC_DRAW, vertices.size(), vertices.data());
		else
			_2d_vbo.write(0, vertices.size(), vertices.data());

		if (indices.size() > _2d_ibo.size())
			_2d_ibo.initialize(GL_DYNAMIC_DRAW, indices.size(), indices.data());
		else
			_2d_ibo.write(0, indices.size(), indices.data());

		_2d_vao.bind();
		_2d_ibo.bind();
		_gl.DrawElements(GL_TRIANGLE_STRIP, indices.size() / sizeof(uint16_t), GL_UNSIGNED_SHORT, 0);
		_2d_ibo.unbind();
		_2d_vao.unbind();

		++_statistics._draw_calls;
		_statistics._triangles += indices.size() / sizeof(uint16_t) - 2;
	}

	void GlRenderer::set_program(const GpuProgram* program)
	{
		_gl.UseProgram(program ? static_cast<const GlGpuProgram*>(program)->handle() : 0);
	}

	void GlRenderer::set_texture(const Texture2D& texture)
	{
		static_cast<const GlTexture2D&>(texture).bind();
	}

	void GlRenderer::set_window_size_impl(const Size& size)
	{
		_gl.Viewport(0, 0, size.width(), size.height());
	}

#if Y_IS_DEBUG
	void GlRenderer::debug_callback(GLenum, GLenum type, GLuint, GLenum, GLsizei, const GLchar* message) const
	{
		bool stop = true;
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			std::cerr << "[error] ";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cerr << "[deprecated] ";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cerr << "[undefined behavior] ";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cerr << "[portability warning] ";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cerr << "[performance warning] ";
			break;
		default:
			stop = false;
		}
		std::cerr << message << std::endl;
		if (stop)
			::raise(SIGINT);
	}
#endif
}
