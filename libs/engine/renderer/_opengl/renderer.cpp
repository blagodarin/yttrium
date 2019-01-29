//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "renderer.h"

#include <yttrium/math/matrix.h>
#include <yttrium/math/rect.h>
#include <yttrium/utils/numeric.h>
#include "../image_wrapper.h"
#include "../mesh_data.h"
#include "mesh.h"
#include "program.h"
#include "texture.h"

#include <cassert>

#ifndef NDEBUG
#	include <csignal>
#	include <iostream>
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

namespace Yttrium
{
	GlRenderer::GlRenderer(WindowBackend&)
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
		_2d_vao.vertex_attrib_format(0, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, position));
		_2d_vao.vertex_attrib_binding(1, 0);
		_2d_vao.vertex_attrib_format(1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, color));
		_2d_vao.vertex_attrib_binding(2, 0);
		_2d_vao.vertex_attrib_format(2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, texture));
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
#ifndef NDEBUG
			std::cerr << vertex.info_log() << "\n";
#endif
			return {};
		}

		GlShaderHandle fragment{ _gl, GL_FRAGMENT_SHADER };
		if (!fragment.compile(fragment_shader))
		{
#ifndef NDEBUG
			std::cerr << fragment.info_log() << "\n";
#endif
			return {};
		}

		auto result = std::make_unique<GlProgram>(std::move(vertex), std::move(fragment), _gl);
		if (!result->link())
			return {};

		return result;
	}

	std::unique_ptr<Texture2D> GlRenderer::create_texture_2d(const Image& image, Flags<RenderManager::TextureFlag> flags)
	{
		ImageWrapper wrapper{ image };

		if (flags & RenderManager::TextureFlag::Intensity)
			wrapper.intensity_to_bgra();

		GLenum internal_format = 0;
		GLenum data_format = 0;
		GLenum data_type = 0;
		switch (wrapper->info().pixel_format())
		{
		case PixelFormat::Gray8:
		case PixelFormat::GrayAlpha16:
			wrapper.to_bgra();
			internal_format = GL_RGBA8;
			data_format = GL_BGRA;
			data_type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::Rgb24:
			internal_format = GL_RGB8;
			data_format = GL_RGB;
			data_type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::Bgr24:
			internal_format = GL_RGB8;
			data_format = GL_BGR;
			data_type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::Rgba32:
			internal_format = GL_RGBA8;
			data_format = GL_RGBA;
			data_type = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::Bgra32:
			internal_format = GL_RGBA8;
			data_format = GL_BGRA;
			data_type = GL_UNSIGNED_BYTE;
			break;
		default:
			return {};
		}

		GlTextureHandle texture{ _gl, GL_TEXTURE_2D };
		_gl.PixelStorei(GL_PACK_ALIGNMENT, static_cast<GLint>(max_2_alignment(wrapper->info().stride() | 8)));
		texture.set_data(0, internal_format, static_cast<GLsizei>(wrapper->info().width()), static_cast<GLsizei>(wrapper->info().height()), data_format, data_type, wrapper->data());
		const auto has_mipmaps = !(flags & RenderManager::TextureFlag::NoMipmaps);
		if (has_mipmaps)
			texture.generate_mipmaps();
		return std::make_unique<GlTexture2D>(*this, wrapper->info(), has_mipmaps, std::move(texture));
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

	RectF GlRenderer::map_rect(const RectF& rect, ImageOrientation orientation) const
	{
		const auto map_point = [orientation](const Vector2& point) -> Vector2 {
			return { point.x, orientation == ImageOrientation::XRightYDown ? point.y : 1.f - point.y };
		};
		return { map_point(rect.top_left()), map_point(rect.bottom_right()) };
	}

	void GlRenderer::set_program(const RenderProgram* program)
	{
		_gl.UseProgram(program ? static_cast<const GlProgram*>(program)->handle() : 0);
	}

	void GlRenderer::set_texture(const Texture2D& texture, Flags<Texture2D::Filter> filter)
	{
		static_cast<const GlTexture2D&>(texture).bind(filter);
	}

	void GlRenderer::set_window_size(const Size& size)
	{
		_gl.Viewport(0, 0, size._width, size._height);
	}

	Image GlRenderer::take_screenshot(const Size& window_size) const
	{
		GLint alignment = 0;
		_gl.GetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);

		Image image{ { static_cast<std::size_t>(window_size._width), static_cast<std::size_t>(window_size._height), PixelFormat::Rgb24, static_cast<std::size_t>(alignment), ImageOrientation::XRightYUp } };

		GLint read_buffer = GL_BACK;
		_gl.GetIntegerv(GL_READ_BUFFER, &read_buffer);
		_gl.ReadBuffer(GL_FRONT);
		_gl.ReadPixels(0, 0, window_size._width, window_size._height, GL_RGB, GL_UNSIGNED_BYTE, image.data());
		_gl.ReadBuffer(static_cast<GLenum>(read_buffer));

		return image;
	}

#ifndef NDEBUG
	void GlRenderer::debug_callback(GLenum, GLenum type, GLuint, GLenum, GLsizei, const GLchar* message) const
	{
		bool stop = true;
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			std::cerr << "[OPENGL] [error] ";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cerr << "[OPENGL] [deprecated] ";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cerr << "[OPENGL] [undefined behavior] ";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cerr << "[OPENGL] [portability warning] ";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cerr << "[OPENGL] [performance warning] ";
			break;
		default:
			std::cerr << "[OPENGL] ";
			stop = false;
			break;
		}
		std::cerr << message << "\n";
		if (stop)
			::raise(SIGINT);
	}
#endif
}
