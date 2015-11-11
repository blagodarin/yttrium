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
	GlRenderer::GlRenderer(Allocator& allocator)
		: RendererImpl(allocator)
		, _gl(allocator)
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

	Pointer<IndexBuffer> GlRenderer::create_index_buffer(IndexBuffer::Format format, size_t size, const void* data)
	{
		const size_t element_size = (format == IndexBuffer::Format::U16) ? 2 : 4;
		const size_t gl_format = (format == IndexBuffer::Format::U16) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

		GlBufferHandle buffer(_gl, GL_ELEMENT_ARRAY_BUFFER_ARB);
		buffer.initialize(GL_STATIC_DRAW_ARB, size * element_size, data);
		return make_pointer<GlIndexBuffer>(allocator(), format, size, element_size, std::move(buffer), gl_format);
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

	Pointer<VertexBuffer> GlRenderer::create_vertex_buffer(std::initializer_list<VA> format, size_t size, const void* data)
	{
		const auto element_size = VertexBufferImpl::element_size(format);
		assert(element_size > 0);
		GlBufferHandle buffer(_gl, GL_ARRAY_BUFFER_ARB);
		buffer.initialize(GL_STATIC_DRAW_ARB, size * element_size, data);
		return make_pointer<GlVertexBuffer>(allocator(), format, size, element_size, std::move(buffer), allocator());
	}

	void GlRenderer::draw_triangles(const VertexBuffer& vertex_buffer, const IndexBuffer& index_buffer)
	{
		update_state();

		const auto& vertices = static_cast<const GlVertexBuffer&>(vertex_buffer);
		const auto& indices = static_cast<const GlIndexBuffer&>(index_buffer);

		_gl.Enable(GL_DEPTH_TEST);
		_gl.DepthFunc(GL_LESS);

		vertices._buffer.bind();

		GLuint attribute_index = 0;
		size_t attribute_offset = 0;

		while (attribute_index < vertices.format().size())
		{
			_gl.EnableVertexAttribArray(attribute_index);
			switch (vertices.format()[attribute_index])
			{
			case VA::f:
				_gl.VertexAttribPointer(attribute_index, 1, GL_FLOAT, GL_FALSE, vertices.element_size(), reinterpret_cast<void*>(attribute_offset));
				attribute_offset += sizeof(float);
				break;
			case VA::f2:
				_gl.VertexAttribPointer(attribute_index, 2, GL_FLOAT, GL_FALSE, vertices.element_size(), reinterpret_cast<void*>(attribute_offset));
				attribute_offset += sizeof(float) * 2;
				break;
			case VA::f3:
				_gl.VertexAttribPointer(attribute_index, 3, GL_FLOAT, GL_FALSE, vertices.element_size(), reinterpret_cast<void*>(attribute_offset));
				attribute_offset += sizeof(float) * 3;
				break;
			case VA::f4:
				_gl.VertexAttribPointer(attribute_index, 4, GL_FLOAT, GL_FALSE, vertices.element_size(), reinterpret_cast<void*>(attribute_offset));
				attribute_offset += sizeof(float) * 4;
				break;
			}
			++attribute_index;
		}
		assert(attribute_offset == vertices.element_size());

		indices._buffer.bind();

		_gl.DrawElements(GL_TRIANGLES, indices.size(), indices._gl_format, 0);
		++_statistics._draw_calls;
		_statistics._triangles += indices.size() / 3;

		indices._buffer.unbind();

		while (attribute_index > 0)
			_gl.DisableVertexAttribArray(--attribute_index);

		vertices._buffer.unbind();

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
		image.set_size(size.width, size.height, unpack_alignment);

		GLint read_buffer;
		_gl.GetIntegerv(GL_READ_BUFFER, &read_buffer);
		_gl.ReadBuffer(GL_FRONT);
		_gl.ReadPixels(0, 0, size.width, size.height, GL_RGB, GL_UNSIGNED_BYTE, image.data());
		_gl.ReadBuffer(read_buffer);
	}

	void GlRenderer::flush_2d_impl(const StdVector<Vertex2D>& vertices, const StdVector<uint16_t>& indices)
	{
		update_state();

		_gl.EnableVertexAttribArray(0);
		_gl.VertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), &vertices[0].position);

		_gl.EnableVertexAttribArray(1);
		_gl.VertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), &vertices[0].color);

		if (current_texture_2d())
		{
			_gl.EnableVertexAttribArray(2);
			_gl.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), &vertices[0].texture);
		}

		_gl.DrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_SHORT, indices.data());
		++_statistics._draw_calls;
		_statistics._triangles += indices.size() - 2;

		_gl.DisableVertexAttribArray(2);
		_gl.DisableVertexAttribArray(1);
		_gl.DisableVertexAttribArray(0);
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
		_gl.Viewport(0, 0, size.width, size.height);
	}

#if Y_IS_DEBUG
	void GlRenderer::debug_callback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar* message) const
	{
		std::cerr << message << std::endl;
	}
#endif
}
