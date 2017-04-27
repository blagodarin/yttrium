#include "renderer.h"

#include <yttrium/exceptions.h>
#include <yttrium/math/line.h>
#include <yttrium/math/matrix.h>
#include <yttrium/math/quad.h>
#include <yttrium/memory/buffer_appender.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/mesh.h>
#include <yttrium/renderer/textured_rect.h>
#include "debug_renderer.h"
#include "debug_texture.h"
#include "formats/obj.h"
#include "mesh_data.h"
#include "gl/renderer.h"
#include "texture.h"

#include <algorithm>
#include <cassert>

namespace Yttrium
{
	const std::string _vertex_shader_2d =
		"#version 330\n"
		"\n"
		"layout(location = 0) in vec2 in_position;\n"
		"layout(location = 1) in vec4 in_color;\n"
		"layout(location = 2) in vec2 in_texcoord;\n"
		"\n"
		"uniform mat4 mvp;\n"
		"\n"
		"out vec4 io_color;\n"
		"out vec2 io_texcoord;\n"
		"\n"
		"void main()\n"
		"{\n"
			"gl_Position = mvp * vec4(in_position, 0, 1);\n"
			"io_color = in_color;\n"
			"io_texcoord = in_texcoord;\n"
		"}\n";

	const std::string _fragment_shader_2d =
		"#version 330\n"
		"\n"
		"in vec4 io_color;\n"
		"in vec2 io_texcoord;\n"
		"\n"
		"uniform sampler2D surface_texture;\n"
		"\n"
		"void main()\n"
		"{\n"
			"gl_FragColor = io_color * texture2D(surface_texture, io_texcoord);\n"
		"}\n";

	std::unique_ptr<RendererImpl> RendererImpl::create(WindowBackend&)
	{
		auto renderer = std::make_unique<GlRenderer>();

		static const int32_t white_texture_data = -1;
		renderer->_white_texture = renderer->create_texture_2d({ { 1, 1, PixelFormat::Bgra, 32 }, &white_texture_data }, TextureFlag::NoMipmaps);
		if (!renderer->_white_texture)
			throw InitializationError("Failed to initialize an internal texture");

		renderer->_debug_texture = renderer->create_texture_2d({ { DebugTexture::width, DebugTexture::height, PixelFormat::Bgra, 32 }, DebugTexture::data }, TextureFlag::NoMipmaps);
		if (!renderer->_debug_texture)
			throw InitializationError("Failed to initialize an internal texture");

		renderer->_program_2d = renderer->create_gpu_program(_vertex_shader_2d, _fragment_shader_2d);
		if (!renderer->_program_2d)
			throw InitializationError("Failed to initialize an internal GPU program");

		return std::move(renderer);
	}

	RendererImpl::~RendererImpl() = default;

	void RendererImpl::draw_debug_text(const std::string& text)
	{
		if (text.empty())
			return;

		DebugRenderer debug(*this);
		debug.set_color(1, 1, 1);
		int top = 0;
		size_t line_begin = 0;
		auto line_end = text.find('\n', line_begin);
		while (line_end != std::string::npos)
		{
			debug.draw_text(0, top++, StaticString{text.data() + line_begin, line_end - line_begin});
			line_begin = line_end + 1;
			line_end = text.find('\n', line_begin);
		}
		debug.draw_text(0, top, StaticString{text.data() + line_begin, text.size() - line_begin});
	}

	void RendererImpl::draw_quad(const Quad& quad, const Color4f& color)
	{
		// TODO: Use one intro code for draw_quad and draw_rect.

		BufferAppender<Vertex2D> vertices(_vertices_2d);
		BufferAppender<uint16_t> indices(_indices_2d);

		auto index = static_cast<uint16_t>(vertices.count()); // TODO: Check size.

		if (index > 0)
		{
			indices << (index - 1) << (index);
			if (indices.count() & 1)
				indices << (index); // Add an extra degenerate to ensure the correct face ordering.
		}

		Vertex2D vertex;
		vertex.color = color;

		vertex.position = quad._a;
		vertex.texture = {_texture_rect.top_left()._x, _texture_rect.top_left()._y};
		vertices << vertex;

		vertex.position = quad._b;
		vertex.texture = {_texture_rect.top_right()._x, _texture_rect.top_right()._y};
		vertices << vertex;

		vertex.position = quad._c;
		vertex.texture = {_texture_rect.bottom_right()._x, _texture_rect.bottom_right()._y};
		vertices << vertex;

		vertex.position = quad._d;
		vertex.texture = {_texture_rect.bottom_left()._x, _texture_rect.bottom_left()._y};
		vertices << vertex;

		indices << index << index + 3 << index + 1 << index + 2;
	}

	void RendererImpl::draw_rect(const RectF& rect, const Color4f& color)
	{
		draw_rect(rect, color, _texture_rect, _texture_borders);
	}

	void RendererImpl::draw_rects(const std::vector<TexturedRect>& rects, const Color4f& color)
	{
		const SizeF texture_size{current_texture_2d()->size()};
		const Vector2 texture_scale{texture_size._width, texture_size._height};
		for (const auto& rect : rects)
			draw_rect(rect.geometry, color, map_rect(rect.texture / texture_scale, current_texture_2d()->orientation()), {});
	}

	Matrix4 RendererImpl::full_matrix() const
	{
		const auto current_projection = std::find_if(_matrix_stack.rbegin(), _matrix_stack.rend(), [](const auto& m){ return m.second == MatrixType::Projection; });
		assert(current_projection != _matrix_stack.rend());
		const auto current_view = current_projection.base();
		assert(current_view != _matrix_stack.end());
		assert(current_view->second == MatrixType::View);
		return current_projection->first * current_view->first * model_matrix();
	}

	std::unique_ptr<Mesh> RendererImpl::load_mesh(const Source& source)
	{
		return create_mesh(load_obj_mesh(source));
	}

	Matrix4 RendererImpl::model_matrix() const
	{
		assert(!_matrix_stack.empty());
		assert(_matrix_stack.back().second == MatrixType::Model);
		return _matrix_stack.back().first;
	}

	Line3 RendererImpl::pixel_ray(const PointF& p) const
	{
		// Move each coordinate to the center of the pixel (by adding 0.5), then normalize from [0, D] to [-1, 1].
		const auto xn = (2 * p._x + 1) / _window_size._width - 1;
		const auto yn = 1 - (2 * p._y + 1) / _window_size._height;
		const auto m = inverse(full_matrix());
		return {m * Vector3{xn, yn, 0}, m * Vector3{xn, yn, 1}};
	}

	void RendererImpl::set_texture_rect(const RectF& rect, const Margins& borders)
	{
		const auto current_texture = current_texture_2d();
		if (!current_texture)
			return;

		const SizeF texture_size{current_texture->size()};
		const Vector2 texture_scale{texture_size._width, texture_size._height};
		const auto texture_rect_size = _texture_rect.size();
		const auto minimum = SizeF{min_size(borders)} / texture_scale;
		if (texture_rect_size._width < minimum._width || texture_rect_size._height < minimum._height)
			return;

		_texture_rect = map_rect(rect / texture_scale, current_texture->orientation());
		_texture_borders =
		{
			borders._top / texture_size._height,
			borders._right / texture_size._width,
			borders._bottom / texture_size._height,
			borders._left / texture_size._width,
		};
	}

	const Texture2D* RendererImpl::debug_texture() const
	{
		return _debug_texture.get();
	}

	void RendererImpl::draw_rect(const RectF& position, const Color4f& color, const RectF& texture)
	{
		draw_rect(position, color, texture, {});
	}

	void RendererImpl::forget_program(const GpuProgram* program)
	{
		if (program == _current_program)
			_current_program = nullptr;
	}

	void RendererImpl::forget_texture(const Texture2D* texture)
	{
		if (texture == _current_texture)
			_current_texture = nullptr;
	}

	void RendererImpl::pop_program()
	{
		assert(_program_stack.size() > 1 || (_program_stack.size() == 1 && _program_stack.back().second > 1));
		if (_program_stack.back().second > 1)
		{
			--_program_stack.back().second;
			return;
		}
		flush_2d();
		_program_stack.pop_back();
		_reset_program = true;
	}

	void RendererImpl::pop_projection()
	{
		flush_2d();
		assert(_matrix_stack.size() >= 3);
		assert(_matrix_stack.back().second == MatrixType::Model);
		_matrix_stack.pop_back();
		assert(_matrix_stack.back().second == MatrixType::View);
		_matrix_stack.pop_back();
		assert(_matrix_stack.back().second == MatrixType::Projection);
		_matrix_stack.pop_back();
		if (_matrix_stack.empty())
			return;
		assert(_matrix_stack.back().second == MatrixType::Model);
#ifndef NDEBUG
		const auto last_view = std::find_if(_matrix_stack.rbegin(), _matrix_stack.rend(), [](const auto& m){ return m.second == MatrixType::View; });
		assert(last_view != _matrix_stack.rend());
		const auto last_projection = std::next(last_view);
		assert(last_projection != _matrix_stack.rend());
		assert(last_projection->second == MatrixType::Projection);
#endif
	}

	void RendererImpl::pop_texture(Flags<Texture2D::Filter> filter)
	{
		assert(_texture_stack.size() > 1 || (_texture_stack.size() == 1 && _texture_stack.back().second > 1));
		if (_texture_stack.back().second == 1)
		{
			flush_2d();
			_texture_stack.pop_back();
			_reset_texture = true;
			reset_texture_state();
		}
		else
			--_texture_stack.back().second;
		_current_texture_filter = filter;
	}

	void RendererImpl::pop_transformation()
	{
		flush_2d();
		assert(_matrix_stack.size() > 3);
		assert(_matrix_stack.back().second == MatrixType::Model);
		_matrix_stack.pop_back();
		assert(_matrix_stack.back().second == MatrixType::Model);
	}

	void RendererImpl::push_program(const GpuProgram* program)
	{
		assert(!_program_stack.empty());
		if (_program_stack.back().first == program)
		{
			++_program_stack.back().second;
			return;
		}
		flush_2d();
		_program_stack.emplace_back(program, 1);
		_reset_program = true;
	}

	void RendererImpl::push_projection_2d(const Matrix4& matrix)
	{
		flush_2d();
		_matrix_stack.emplace_back(matrix, MatrixType::Projection);
		_matrix_stack.emplace_back(Matrix4::identity(), MatrixType::View);
		_matrix_stack.emplace_back(Matrix4::identity(), MatrixType::Model);
	}

	const Matrix4 _3d_directions // Makes Y point forward and Z point up.
	{
		1,  0,  0,  0,
		0,  0,  1,  0,
		0, -1,  0,  0,
		0,  0,  0,  1,
	};

	void RendererImpl::push_projection_3d(const Matrix4& projection, const Matrix4& view)
	{
		flush_2d();
		_matrix_stack.emplace_back(projection, MatrixType::Projection);
		_matrix_stack.emplace_back(_3d_directions * view, MatrixType::View);
		_matrix_stack.emplace_back(Matrix4::identity(), MatrixType::Model);
	}

	Flags<Texture2D::Filter> RendererImpl::push_texture(const Texture2D* texture, Flags<Texture2D::Filter> filter)
	{
		if (!texture)
		{
			texture = _white_texture.get();
			filter = Texture2D::NearestFilter;
		}
		assert(!_texture_stack.empty());
		if (_texture_stack.back().first != texture)
		{
			flush_2d();
			_texture_stack.emplace_back(texture, 1);
			_reset_texture = true;
			reset_texture_state();
		}
		else
			++_texture_stack.back().second;
		return std::exchange(_current_texture_filter, filter);
	}

	void RendererImpl::push_transformation(const Matrix4& matrix)
	{
		assert(!_matrix_stack.empty());
		assert(_matrix_stack.back().second == MatrixType::Model);
		_matrix_stack.emplace_back(_matrix_stack.back().first * matrix, MatrixType::Model);
	}

	RendererImpl::Statistics RendererImpl::reset_statistics()
	{
		const auto result = _statistics;
		_statistics = {};
#ifndef NDEBUG
		_seen_textures.clear();
		_seen_programs.clear();
#endif
		return result;
	}

	void RendererImpl::set_window_size(const Size& size)
	{
		_window_size = size;
		set_window_size_impl(_window_size);
	}

	const BackendTexture2D* RendererImpl::current_texture_2d() const
	{
		return static_cast<const BackendTexture2D*>(_texture_stack.back().first);
	}

	void RendererImpl::update_state()
	{
		if (_reset_program)
		{
			_reset_program = false;
			const auto program = _program_stack.back().first;
			if (program != _current_program)
			{
				_current_program = program;
				set_program(program);
				++_statistics._shader_switches;
#ifndef NDEBUG
				const auto i = std::find(_seen_programs.begin(), _seen_programs.end(), program);
				if (i == _seen_programs.end())
					_seen_programs.emplace_back(program);
				else
					++_statistics._redundant_shader_switches;
#endif
			}
		}

		if (_reset_texture)
		{
			_reset_texture = false;
			const auto texture = _texture_stack.back().first;
			if (texture != _current_texture)
			{
				_current_texture = texture;
				set_texture(*texture, _current_texture_filter);
				++_statistics._texture_switches;
#ifndef NDEBUG
				const auto i = std::find(_seen_textures.begin(), _seen_textures.end(), texture);
				if (i == _seen_textures.end())
					_seen_textures.emplace_back(texture);
				else
					++_statistics._redundant_texture_switches;
#endif
			}
		}
	}

	void RendererImpl::draw_rect(const RectF& position, const Color4f& color, const RectF& texture, const MarginsF& borders)
	{
		BufferAppender<Vertex2D> vertices(_vertices_2d);
		BufferAppender<uint16_t> indices(_indices_2d);

		auto index = static_cast<uint16_t>(vertices.count()); // TODO: Check size.

		if (index > 0)
		{
			indices << (index - 1) << (index);
			if (indices.count() & 1)
				indices << (index); // Add an extra degenerate to ensure the correct face ordering.
		}

		Vertex2D vertex;
		vertex.color = color;

		float left_offset = 0;
		float right_offset = 0;

		// Outer top vertex row.

		vertex.position.y = position.top();
		vertex.texture.y = texture.top();

		vertex.position.x = position.left();
		vertex.texture.x = texture.left();
		vertices << vertex;

		if (borders._left > 0)
		{
			left_offset = borders._left * current_texture_2d()->size()._width;

			vertex.position.x = position.left() + left_offset;
			vertex.texture.x = texture.left() + borders._left;
			vertices << vertex;
		}

		if (borders._right > 0)
		{
			right_offset = borders._right * current_texture_2d()->size()._width;

			vertex.position.x = position.right() - right_offset;
			vertex.texture.x = texture.right() - borders._right;
			vertices << vertex;
		}

		vertex.position.x = position.right();
		vertex.texture.x = texture.right();
		vertices << vertex;

		// Top/only part indices.

		const uint16_t row_vertices = vertices.count() - index;

		for (uint16_t i = 0; i < row_vertices; ++i)
			indices << (index + i) << (index + i + row_vertices);

		if (borders._top > 0)
		{
			float top_offset = borders._top * current_texture_2d()->size()._height;

			// Inner top vertex row.

			vertex.position.y = position.top() + top_offset;
			vertex.texture.y = texture.top() + borders._top;

			vertex.position.x = position.left();
			vertex.texture.x = texture.left();
			vertices << vertex;

			if (borders._left > 0)
			{
				vertex.position.x = position.left() + left_offset;
				vertex.texture.x = texture.left() + borders._left;
				vertices << vertex;
			}

			if (borders._right > 0)
			{
				vertex.position.x = position.right() - right_offset;
				vertex.texture.x = texture.right() - borders._right;
				vertices << vertex;
			}

			vertex.position.x = position.right();
			vertex.texture.x = texture.right();
			vertices << vertex;

			// Middle/bottom part indices.

			index += row_vertices;

			indices << (index + row_vertices - 1) << (index);
			for (uint16_t i = 0; i < row_vertices; ++i)
				indices << (index + i) << (index + i + row_vertices);
		}

		if (borders._bottom > 0)
		{
			float bottom_offset = borders._bottom * current_texture_2d()->size()._height;

			// Inner bottom vertex row.

			vertex.position.y = position.bottom() - bottom_offset;
			vertex.texture.y = texture.bottom() - borders._bottom;

			vertex.position.x = position.left();
			vertex.texture.x = texture.left();
			vertices << vertex;

			if (borders._left > 0)
			{
				vertex.position.x = position.left() + left_offset;
				vertex.texture.x = texture.left() + borders._left;
				vertices << vertex;
			}

			if (borders._right > 0)
			{
				vertex.position.x = position.right() - right_offset;
				vertex.texture.x = texture.right() - borders._right;
				vertices << vertex;
			}

			vertex.position.x = position.right();
			vertex.texture.x = texture.right();
			vertices << vertex;

			// Bottom part indices.

			index += row_vertices;

			indices << (index + row_vertices - 1) << (index);
			for (size_t i = 0; i < row_vertices; ++i)
				indices << (index + i) << (index + i + row_vertices);
		}

		// Outer bottom vertex row.

		vertex.position.y = position.bottom();
		vertex.texture.y = texture.bottom();

		vertex.position.x = position.left();
		vertex.texture.x = texture.left();
		vertices << vertex;

		if (borders._left > 0)
		{
			vertex.position.x = position.left() + left_offset;
			vertex.texture.x = texture.left() + borders._left;
			vertices << vertex;
		}

		if (borders._right > 0)
		{
			vertex.position.x = position.right() - right_offset;
			vertex.texture.x = texture.right() - borders._right;
			vertices << vertex;
		}

		vertex.position.x = position.right();
		vertex.texture.x = texture.right();
		vertices << vertex;
	}

	void RendererImpl::flush_2d()
	{
		if (_vertices_2d.size() > 0)
		{
			_program_2d->set_uniform("mvp", full_matrix());
			flush_2d_impl(_vertices_2d, _indices_2d);
			_vertices_2d.resize(0);
			_indices_2d.resize(0);
		}
	}

	void RendererImpl::reset_texture_state()
	{
		const auto* texture = current_texture_2d();
		_texture_rect = texture ? texture->full_rectangle() : RectF();
		_texture_borders = {};
	}
}
