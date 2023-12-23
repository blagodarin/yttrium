// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "renderer.h"

#include <yttrium/base/logger.h>
#include "../../2d.h"
#include "../../model/mesh_data.h"
#include "mesh.h"
#include "program.h"
#include "texture.h"

#include <seir_base/int_utils.hpp>
#include <seir_graphics/rectf.hpp>
#include <seir_image/image.hpp>
#include <seir_image/utils.hpp>

#include <cassert>

#ifndef NDEBUG
#	include <csignal>
#	include <fmt/format.h>
#endif

namespace
{
	const std::string _vertex_shader_2d =
#include "2d_vs.glsl.inc"
		;

	const std::string _fragment_shader_2d =
#include "2d_fs.glsl.inc"
		;
}

namespace Yt
{
	GlRenderer::GlRenderer(const WindowID&)
	{
#ifndef NDEBUG
		if (_gl.KHR_debug)
		{
			_gl.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			_gl.DebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* opaque) {
				static_cast<const GlRenderer*>(opaque)->debug_callback(source, type, id, severity, length, message);
			},
				this);
		}
#endif
		_gl.Enable(GL_CULL_FACE); // The default behavior is to cull back (clockwise) faces.
		_gl.Enable(GL_BLEND);
		_gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		_gl.ClearColor(0.125, 0.125, 0.125, 0);
		_gl.ClearDepth(1);

		_2d_vbo.initialize(GL_DYNAMIC_DRAW, 0, nullptr); // The next line fails without this.
		_2d_vao.bind_vertex_buffer(0, _2d_vbo.get(), 0, sizeof(Vertex2D));
		_2d_vao.vertex_attrib_binding(0, 0);
		_2d_vao.vertex_attrib_format(0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, _position));
		_2d_vao.vertex_attrib_binding(1, 0);
		_2d_vao.vertex_attrib_format(1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, _texture));
		_2d_vao.vertex_attrib_binding(2, 0);
		_2d_vao.vertex_attrib_format(2, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE, offsetof(Vertex2D, _color));
	}

	GlRenderer::~GlRenderer() noexcept = default;

	void GlRenderer::clear()
	{
		_gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	std::unique_ptr<RenderProgram> GlRenderer::create_builtin_program_2d()
	{
		return create_program(_vertex_shader_2d, _fragment_shader_2d);
	}

	std::unique_ptr<Mesh> GlRenderer::create_mesh(const MeshData& data)
	{
		GlVertexArrayHandle vertex_array(_gl);
		GLuint index = 0;
		size_t offset = 0;
		for (const auto type : data._vertex_format)
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

		GlBufferHandle vertex_buffer(_gl, GL_ARRAY_BUFFER);
		vertex_buffer.initialize(GL_STATIC_DRAW, data._vertex_data.size(), data._vertex_data.data());
		vertex_array.bind_vertex_buffer(0, vertex_buffer.get(), 0, offset);

		GlBufferHandle index_buffer(_gl, GL_ELEMENT_ARRAY_BUFFER);
		GLenum index_format = GL_UNSIGNED_INT;
		if (Buffer index_data; data.make_uint16_indices(index_data))
		{
			index_buffer.initialize(GL_STATIC_DRAW, index_data.size(), index_data.data());
			index_format = GL_UNSIGNED_SHORT;
		}
		else
			index_buffer.initialize(GL_STATIC_DRAW, data._indices.size() * sizeof(uint32_t), data._indices.data());

		return std::make_unique<OpenGLMesh>(std::move(vertex_array), std::move(vertex_buffer), std::move(index_buffer), static_cast<GLsizei>(data._indices.size()), index_format);
	}

	std::unique_ptr<RenderProgram> GlRenderer::create_program(const std::string& vertex_shader, const std::string& fragment_shader)
	{
		GlShaderHandle vertex{ _gl, GL_VERTEX_SHADER };
		if (!vertex.compile(vertex_shader))
		{
			Logger::write(vertex.info_log());
			return {};
		}

		GlShaderHandle fragment{ _gl, GL_FRAGMENT_SHADER };
		if (!fragment.compile(fragment_shader))
		{
			Logger::write(fragment.info_log());
			return {};
		}

		auto result = std::make_unique<GlProgram>(std::move(vertex), std::move(fragment), _gl);
		if (!result->link())
			return {};

		return result;
	}

	std::unique_ptr<Texture2D> GlRenderer::create_texture_2d(const seir::ImageInfo& image_info, const void* image_data, Flags<RenderManager::TextureFlag> flags)
	{
		const auto create = [this, flags](const seir::ImageInfo& info, const void* data, std::size_t alignment) -> std::unique_ptr<Texture2D> {
			GlTextureHandle texture{ _gl, GL_TEXTURE_2D };
			_gl.PixelStorei(GL_PACK_ALIGNMENT, static_cast<GLint>(alignment));
			texture.set_data(0, GL_RGBA8, static_cast<GLsizei>(info.width()), static_cast<GLsizei>(info.height()), GL_BGRA, GL_UNSIGNED_BYTE, data);
			const auto has_mipmaps = !(flags & RenderManager::TextureFlag::NoMipmaps);
			if (has_mipmaps)
				texture.generate_mipmaps();
			return std::make_unique<GlTexture2D>(*this, info, has_mipmaps, std::move(texture));
		};

		if (image_info.pixelFormat() == seir::PixelFormat::Bgra32)
			if (const auto alignment = seir::powerOf2Alignment(image_info.stride() | 8); alignment == 4 || alignment == 8)
				return create(image_info, image_data, alignment);

		const seir::ImageInfo transformed_info{ image_info.width(), image_info.height(), seir::PixelFormat::Bgra32, image_info.axes() };
		Buffer buffer{ transformed_info.frameSize() };
		if (!seir::copyImage(image_info, image_data, transformed_info, buffer.data()))
			return {};

		// TODO: Count "slow" textures.
		return create(transformed_info, buffer.data(), seir::powerOf2Alignment(transformed_info.stride() | 8));
	}

	size_t GlRenderer::draw_mesh(const Mesh& mesh)
	{
		const auto& opengl_mesh = static_cast<const OpenGLMesh&>(mesh);

		_gl.Enable(GL_DEPTH_TEST);
		_gl.DepthFunc(GL_LESS);

		opengl_mesh._vertex_array.bind();
		opengl_mesh._index_buffer.bind();
		_gl.DrawElements(GL_TRIANGLES, opengl_mesh._index_buffer_size, opengl_mesh._index_buffer_format, nullptr);
		opengl_mesh._index_buffer.unbind();
		opengl_mesh._vertex_array.unbind();

		_gl.Disable(GL_DEPTH_TEST);

		return static_cast<size_t>(opengl_mesh._index_buffer_size / 3);
	}

	void GlRenderer::flush_2d(const Buffer& vertices, const Buffer& indices) noexcept
	{
		if (vertices.capacity() > _2d_vbo.size())
			_2d_vbo.initialize(GL_DYNAMIC_DRAW, vertices.capacity(), vertices.data());
		else
			_2d_vbo.write(0, vertices.size(), vertices.data());

		if (indices.capacity() > _2d_ibo.size())
			_2d_ibo.initialize(GL_DYNAMIC_DRAW, indices.capacity(), indices.data());
		else
			_2d_ibo.write(0, indices.size(), indices.data());

		_2d_vao.bind();
		_2d_ibo.bind();
		_gl.DrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(indices.size() / sizeof(uint16_t)), GL_UNSIGNED_SHORT, nullptr);
		_2d_ibo.unbind();
		_2d_vao.unbind();
	}

	seir::RectF GlRenderer::map_rect(const seir::RectF& rect, seir::ImageAxes axes) const
	{
		const auto map_point = [axes](const seir::Vec2& point) -> seir::Vec2 {
			return { point.x, axes == seir::ImageAxes::XRightYDown ? point.y : 1.f - point.y };
		};
		return { map_point(rect.topLeft()), map_point(rect.bottomRight()) };
	}

	void GlRenderer::set_program(const RenderProgram* program)
	{
		_gl.UseProgram(program ? static_cast<const GlProgram*>(program)->handle() : 0);
	}

	void GlRenderer::set_texture(const Texture2D& texture, Flags<Texture2D::Filter> filter)
	{
		static_cast<const GlTexture2D&>(texture).bind(filter);
	}

	void GlRenderer::set_viewport_size(const seir::Size& size)
	{
		_gl.Viewport(0, 0, size._width, size._height);
	}

	seir::Image GlRenderer::take_screenshot(const seir::Size& viewport_size) const
	{
		GLint alignment = 0;
		_gl.GetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);

		const auto width = static_cast<uint32_t>(viewport_size._width);
		const auto height = static_cast<uint32_t>(viewport_size._height);
		const auto stride = (width * 3 + alignment - 1) / alignment * alignment;
		const seir::ImageInfo info{ width, height, stride, seir::PixelFormat::Rgb24, seir::ImageAxes::XRightYUp };
		seir::Buffer buffer{ info.frameSize() };

		GLint read_buffer = GL_BACK;
		_gl.GetIntegerv(GL_READ_BUFFER, &read_buffer);
		_gl.ReadBuffer(GL_FRONT);
		_gl.ReadPixels(0, 0, viewport_size._width, viewport_size._height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
		_gl.ReadBuffer(static_cast<GLenum>(read_buffer));

		return seir::Image{ info, std::move(buffer) };
	}

#ifndef NDEBUG
	void GlRenderer::debug_callback(GLenum, GLenum type, GLuint, GLenum, GLsizei, const GLchar* message) const
	{
		bool stop = true;
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			Logger::write(fmt::format("(OpenGL) Error! {}", message));
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			Logger::write(fmt::format("(OpenGL) Deprecated behavior! {}", message));
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			Logger::write(fmt::format("(OpenGL) Undefined behavior! {}", message));
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			Logger::write(fmt::format("(OpenGL) Portability warning! {}", message));
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			Logger::write(fmt::format("(OpenGL) Performance warning! {}", message));
			break;
		default:
			Logger::write(fmt::format("(OpenGL) ", message));
			stop = false;
			break;
		}
		if (stop)
		{
			Logger::flush();
			::raise(SIGINT);
		}
	}
#endif
}
