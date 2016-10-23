#include "renderer.h"

#include <yttrium/exceptions.h>
#include <yttrium/math/matrix.h>
#include <yttrium/memory/buffer_appender.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/textured_rect.h>
#include "debug_renderer.h"
#include "debug_texture.h"
#include "gl/renderer.h"
#include "texture.h"

#include <algorithm>
#include <cassert>

namespace Yttrium
{
	const auto _vertex_shader_2d =
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
		"}\n"_s;

	const auto _fragment_shader_2d =
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
		"}\n"_s;

	std::unique_ptr<RendererImpl> RendererImpl::create(WindowBackend&, Allocator& allocator)
	{
		auto renderer = std::make_unique<GlRenderer>(allocator);

		static const int32_t white_texture_data = -1;

		ImageFormat white_texture_format;
		white_texture_format.set_width(1);
		white_texture_format.set_height(1);
		white_texture_format.set_orientation(ImageOrientation::XRightYDown);
		white_texture_format.set_pixel_format(PixelFormat::Bgra, 32);
		renderer->_white_texture = renderer->create_texture_2d(white_texture_format, &white_texture_data, false);
		if (!renderer->_white_texture)
			throw InitializationError("Failed to initialize an internal texture");

		ImageFormat debug_texture_format;
		debug_texture_format.set_width(DebugTexture::width);
		debug_texture_format.set_height(DebugTexture::height);
		debug_texture_format.set_orientation(ImageOrientation::XRightYDown);
		debug_texture_format.set_pixel_format(PixelFormat::Bgra, 32);
		renderer->_debug_texture = renderer->create_texture_2d(debug_texture_format, DebugTexture::data, false);
		if (!renderer->_debug_texture)
			throw InitializationError("Failed to initialize an internal texture");

		renderer->_program_2d = renderer->create_gpu_program(_vertex_shader_2d, _fragment_shader_2d);
		if (!renderer->_program_2d)
			throw InitializationError("Failed to initialize an internal GPU program");

		return std::move(renderer);
	}

	RendererImpl::RendererImpl(Allocator& allocator)
		: _allocator(allocator)
		, _matrix_stack(_allocator)
		, _texture_stack({{nullptr, 1}}, _allocator)
#if Y_IS_DEBUG
		, _seen_textures(_allocator)
#endif
		, _program_stack({{nullptr, 1}}, _allocator)
#if Y_IS_DEBUG
		, _seen_programs(_allocator)
#endif
	{
	}

	RendererImpl::~RendererImpl() = default;

	void RendererImpl::draw_debug_text(const StaticString& text)
	{
		if (text.is_empty())
			return;

		DebugRenderer debug(*this);
		debug.set_color(1, 1, 1);
		int top = 0;
		size_t line_begin = 0;
		auto line_end = text.find_first('\n', line_begin);
		while (line_end != StaticString::End)
		{
			debug.draw_text(0, top++, text.mid(line_begin, line_end - line_begin));
			line_begin = line_end + 1;
			line_end = text.find_first('\n', line_begin);
		}
		debug.draw_text(0, top, text.mid(line_begin));
	}

	void RendererImpl::draw_rect(const RectF& rect)
	{
		draw_rect(rect, _texture_rect, _texture_borders);
	}

	void RendererImpl::draw_rects(const StdVector<TexturedRect>& rects)
	{
		const auto& texture_size = SizeF(current_texture_2d()->size());
		const auto& texture_scale = std::make_pair(texture_size.width(), texture_size.height());
		for (const auto& rect : rects)
			draw_rect(rect.geometry, map_rect(rect.texture / texture_scale, current_texture_2d()->orientation()), {});
	}

	Matrix4 RendererImpl::current_projection() const
	{
		const auto last_projection = std::find_if(_matrix_stack.rbegin(), _matrix_stack.rend(),
			[](const auto& matrix) { return matrix.second == MatrixType::Projection; });
		assert(last_projection != _matrix_stack.rend());
		return last_projection->first;
	}

	Matrix4 RendererImpl::current_transformation() const
	{
		assert(!_matrix_stack.empty());
		assert(_matrix_stack.back().second == MatrixType::Transformation);
		return _matrix_stack.back().first;
	}

	void RendererImpl::set_color(const Vector4& color)
	{
		_color = color;
	}

	void RendererImpl::set_texture_rect(const RectF& rect, const Margins& borders)
	{
		const auto current_texture = current_texture_2d();
		if (!current_texture)
			return;

		const auto& texture_size = SizeF(current_texture->size());
		const auto& texture_scale = std::make_pair(texture_size.width(), texture_size.height());
		const auto& texture_rect_size = _texture_rect.size();
		const auto& min_size = SizeF(borders.min_size()) / texture_scale;
		if (texture_rect_size.width() < min_size.width() || texture_rect_size.height() < min_size.height())
			return;

		_texture_rect = map_rect(rect / texture_scale, current_texture->orientation());
		_texture_borders =
		{
			borders.top() / texture_size.height(),
			borders.right() / texture_size.width(),
			borders.bottom() / texture_size.height(),
			borders.left() / texture_size.width()
		};
	}

	const Texture2D* RendererImpl::debug_texture() const
	{
		return _debug_texture.get();
	}

	void RendererImpl::draw_rect(const RectF& position, const RectF& texture)
	{
		draw_rect(position, texture, {});
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
		assert(_matrix_stack.size() >= 2);
		assert(_matrix_stack.back().second == MatrixType::Transformation);
		_matrix_stack.pop_back();
		assert(_matrix_stack.back().second == MatrixType::Projection);
		_matrix_stack.pop_back();
		if (_matrix_stack.empty())
			return;
		assert(_matrix_stack.back().second == MatrixType::Transformation);
	#ifndef NDEBUG
		const auto last_projection = std::find_if(_matrix_stack.rbegin(), _matrix_stack.rend(),
			[](const auto& matrix) { return matrix.second == MatrixType::Projection; });
		assert(last_projection != _matrix_stack.rend());
	#endif
	}

	void RendererImpl::pop_texture(Texture2D::Filter filter)
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
		assert(_matrix_stack.size() >= 3); // Projection, default transformation, transformation to pop.
		assert(_matrix_stack.back().second == MatrixType::Transformation);
		_matrix_stack.pop_back();
		assert(_matrix_stack.back().second == MatrixType::Transformation);
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
		_matrix_stack.emplace_back(Matrix4(), MatrixType::Transformation);
	}

	void RendererImpl::push_projection_3d(const Matrix4& matrix)
	{
		flush_2d();
		_matrix_stack.emplace_back(matrix, MatrixType::Projection);
		// Make Y point forward and Z point up.
		_matrix_stack.emplace_back(Matrix4(
			1,  0,  0,  0,
			0,  0,  1,  0,
			0, -1,  0,  0,
			0,  0,  0,  1), MatrixType::Transformation);
	}

	Texture2D::Filter RendererImpl::push_texture(const Texture2D* texture, Texture2D::Filter filter)
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
		const auto previous_filter = _current_texture_filter;
		_current_texture_filter = filter;
		return previous_filter;
	}

	void RendererImpl::push_transformation(const Matrix4& matrix)
	{
		assert(_matrix_stack.size() >= 2 && _matrix_stack.back().second == MatrixType::Transformation);
		_matrix_stack.emplace_back(_matrix_stack.back().first * matrix, MatrixType::Transformation);
	}

	RendererImpl::Statistics RendererImpl::reset_statistics()
	{
		const auto result = _statistics;
		_statistics = {};
	#if Y_IS_DEBUG
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
			#if Y_IS_DEBUG
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
			#if Y_IS_DEBUG
				const auto i = std::find(_seen_textures.begin(), _seen_textures.end(), texture);
				if (i == _seen_textures.end())
					_seen_textures.emplace_back(texture);
				else
					++_statistics._redundant_texture_switches;
			#endif
			}
		}
	}

	void RendererImpl::draw_rect(const RectF& position, const RectF& texture, const MarginsF& borders)
	{
		BufferAppender<Vertex2D> vertices(_vertices_2d);
		BufferAppender<uint16_t> indices(_indices_2d);

		uint16_t index = vertices.count();

		if (index > 0)
		{
			indices << (index - 1) << (index);
			if (indices.count() & 1)
				indices << (index); // Add an extra degenerate to ensure the correct face ordering.
		}

		Vertex2D vertex;
		vertex.color = _color;

		float left_offset = 0;
		float right_offset = 0;

		// Outer top vertex row.

		vertex.position.y = position.top();
		vertex.texture.y = texture.top();

		vertex.position.x = position.left();
		vertex.texture.x = texture.left();
		vertices << vertex;

		if (borders.left() > 0)
		{
			left_offset = borders.left() * current_texture_2d()->size().width();

			vertex.position.x = position.left() + left_offset;
			vertex.texture.x = texture.left() + borders.left();
			vertices << vertex;
		}

		if (borders.right() > 0)
		{
			right_offset = borders.right() * current_texture_2d()->size().width();

			vertex.position.x = position.right() - right_offset;
			vertex.texture.x = texture.right() - borders.right();
			vertices << vertex;
		}

		vertex.position.x = position.right();
		vertex.texture.x = texture.right();
		vertices << vertex;

		// Top/only part indices.

		const uint16_t row_vertices = vertices.count() - index;

		for (uint16_t i = 0; i < row_vertices; ++i)
			indices << (index + i) << (index + i + row_vertices);

		if (borders.top() > 0)
		{
			float top_offset = borders.top() * current_texture_2d()->size().height();

			// Inner top vertex row.

			vertex.position.y = position.top() + top_offset;
			vertex.texture.y = texture.top() + borders.top();

			vertex.position.x = position.left();
			vertex.texture.x = texture.left();
			vertices << vertex;

			if (borders.left() > 0)
			{
				vertex.position.x = position.left() + left_offset;
				vertex.texture.x = texture.left() + borders.left();
				vertices << vertex;
			}

			if (borders.right() > 0)
			{
				vertex.position.x = position.right() - right_offset;
				vertex.texture.x = texture.right() - borders.right();
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

		if (borders.bottom() > 0)
		{
			float bottom_offset = borders.bottom() * current_texture_2d()->size().height();

			// Inner bottom vertex row.

			vertex.position.y = position.bottom() - bottom_offset;
			vertex.texture.y = texture.bottom() - borders.bottom();

			vertex.position.x = position.left();
			vertex.texture.x = texture.left();
			vertices << vertex;

			if (borders.left() > 0)
			{
				vertex.position.x = position.left() + left_offset;
				vertex.texture.x = texture.left() + borders.left();
				vertices << vertex;
			}

			if (borders.right() > 0)
			{
				vertex.position.x = position.right() - right_offset;
				vertex.texture.x = texture.right() - borders.right();
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

		if (borders.left() > 0)
		{
			vertex.position.x = position.left() + left_offset;
			vertex.texture.x = texture.left() + borders.left();
			vertices << vertex;
		}

		if (borders.right() > 0)
		{
			vertex.position.x = position.right() - right_offset;
			vertex.texture.x = texture.right() - borders.right();
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
			_program_2d->set_uniform("mvp", current_projection() * current_transformation());
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
