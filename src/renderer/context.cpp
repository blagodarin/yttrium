#include "context.h"

#include <yttrium/math/line.h>
#include <yttrium/math/matrix.h>
#include <yttrium/math/quad.h>
#include <yttrium/memory/buffer_appender.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/textured_rect.h>
#include <yttrium/string_utils.h>
#include "debug_renderer.h"
#include "backend.h"
#include "renderer.h"
#include "texture.h"

#include <cassert>

namespace
{
	using namespace Yttrium;

	struct Draw2D
	{
		BufferAppender<RendererBackend::Vertex2D> _vertices;
		BufferAppender<uint16_t> _indices;
		uint16_t _index = static_cast<uint16_t>(_vertices.count());

		Draw2D(Buffer& vertices, Buffer& indices)
			: _vertices{vertices}
			, _indices{indices}
		{
			if (_index > 0)
			{
				_indices << static_cast<uint16_t>(_index - 1) << (_index);
				if (_indices.count() & 1)
					_indices << (_index); // Extra degenerate to ensure correct face vertex ordering.
			}
		}
	};

	const Matrix4 _3d_directions // Makes Y point forward and Z point up.
	{
		1,  0,  0,  0,
		0,  0,  1,  0,
		0, -1,  0,  0,
		0,  0,  0,  1,
	};
}

namespace Yttrium
{
	RenderContextImpl::RenderContextImpl(RendererImpl& renderer, const Size& window_size)
		: _renderer{renderer}
		, _window_size{window_size}
	{
		_renderer._debug_text.clear();
		_renderer._backend->clear();
	}

	RenderContextImpl::~RenderContextImpl() noexcept
	{
#ifndef NDEBUG
		_renderer._seen_textures.clear();
		_renderer._seen_programs.clear();
#endif
	}

	void RenderContextImpl::add_debug_text(std::string_view text)
	{
		append_to(_renderer._debug_text, text, '\n');
	}

	void RenderContextImpl::draw_mesh(const Mesh& mesh)
	{
		update_state();
		_statistics._triangles += _renderer._backend->draw_mesh(mesh);
		++_statistics._draw_calls;
	}

	void RenderContextImpl::draw_quad(const Quad& quad, const Color4f& color)
	{
		Draw2D draw{_renderer._vertices_2d, _renderer._indices_2d};

		RendererBackend::Vertex2D vertex;
		vertex.color = color;

		vertex.position = quad._a;
		vertex.texture = _texture_rect.top_left();
		draw._vertices << vertex;

		vertex.position = quad._d;
		vertex.texture = _texture_rect.bottom_left();
		draw._vertices << vertex;

		vertex.position = quad._b;
		vertex.texture = _texture_rect.top_right();
		draw._vertices << vertex;

		vertex.position = quad._c;
		vertex.texture = _texture_rect.bottom_right();
		draw._vertices << vertex;

		draw._indices << draw._index << static_cast<uint16_t>(draw._index + 1) << static_cast<uint16_t>(draw._index + 2) << static_cast<uint16_t>(draw._index + 3);
	}

	void RenderContextImpl::draw_rect(const RectF& rect, const Color4f& color)
	{
		draw_rect(rect, color, _texture_rect, _texture_borders);
	}

	void RenderContextImpl::draw_rects(const std::vector<TexturedRect>& rects, const Color4f& color)
	{
		const SizeF texture_size{current_texture_2d()->size()};
		const Vector2 texture_scale{texture_size._width, texture_size._height};
		for (const auto& rect : rects)
			draw_rect(rect.geometry, color, _renderer._backend->map_rect(rect.texture / texture_scale, current_texture_2d()->orientation()), {});
	}

	Matrix4 RenderContextImpl::full_matrix() const
	{
		const auto current_projection = std::find_if(_renderer._matrix_stack.rbegin(), _renderer._matrix_stack.rend(), [](const auto& m){ return m.second == RenderMatrixType::Projection; });
		assert(current_projection != _renderer._matrix_stack.rend());
		const auto current_view = current_projection.base();
		assert(current_view != _renderer._matrix_stack.end());
		assert(current_view->second == RenderMatrixType::View);
		return current_projection->first * current_view->first * model_matrix();
	}

	Matrix4 RenderContextImpl::model_matrix() const
	{
		assert(!_renderer._matrix_stack.empty());
		assert(_renderer._matrix_stack.back().second == RenderMatrixType::Model);
		return _renderer._matrix_stack.back().first;
	}

	Line3 RenderContextImpl::pixel_ray(const Vector2& v) const
	{
		// Move each coordinate to the center of the pixel (by adding 0.5), then normalize from [0, D] to [-1, 1].
		const auto xn = (2 * v.x + 1) / static_cast<float>(_window_size._width) - 1;
		const auto yn = 1 - (2 * v.y + 1) / static_cast<float>(_window_size._height);
		const auto m = inverse(full_matrix());
		return {m * Vector3{xn, yn, 0}, m * Vector3{xn, yn, 1}};
	}

	void RenderContextImpl::set_texture_rect(const RectF& rect, const MarginsF& borders)
	{
		const auto current_texture = current_texture_2d();
		if (!current_texture)
			return;

		const SizeF texture_size{current_texture->size()};
		const Vector2 texture_scale{texture_size._width, texture_size._height};
		const auto texture_rect_size = _texture_rect.size();
		const auto minimum = SizeF{borders._left + 1 + borders._right, borders._top + 1 + borders._bottom} / texture_scale;
		if (texture_rect_size._width < minimum._width || texture_rect_size._height < minimum._height)
			return;

		_texture_rect = _renderer._backend->map_rect(rect / texture_scale, current_texture->orientation());
		_texture_borders =
		{
			borders._top / texture_size._height,
			borders._right / texture_size._width,
			borders._bottom / texture_size._height,
			borders._left / texture_size._width,
		};
	}

	SizeF RenderContextImpl::window_size() const
	{
		return SizeF{_window_size};
	}

	void RenderContextImpl::draw_debug_text()
	{
		if (_renderer._debug_text.empty())
			return;
		{
			DebugRenderer debug{*this};
			debug.set_color(1, 1, 1);
			size_t top = 0;
			size_t line_begin = 0;
			auto line_end = _renderer._debug_text.find('\n', line_begin);
			while (line_end != std::string::npos)
			{
				debug.draw_text(0, top++, {_renderer._debug_text.data() + line_begin, line_end - line_begin});
				line_begin = line_end + 1;
				line_end = _renderer._debug_text.find('\n', line_begin);
			}
			debug.draw_text(0, top, {_renderer._debug_text.data() + line_begin, _renderer._debug_text.size() - line_begin});
		}
		flush_2d();
	}

	void RenderContextImpl::pop_program() noexcept
	{
		assert(_renderer._program_stack.size() > 1 || (_renderer._program_stack.size() == 1 && _renderer._program_stack.back().second > 1));
		if (_renderer._program_stack.back().second > 1)
		{
			--_renderer._program_stack.back().second;
			return;
		}
		flush_2d();
		_renderer._program_stack.pop_back();
		_reset_program = true;
	}

	void RenderContextImpl::pop_projection() noexcept
	{
		flush_2d();
		assert(_renderer._matrix_stack.size() >= 3);
		assert(_renderer._matrix_stack.back().second == RenderMatrixType::Model);
		_renderer._matrix_stack.pop_back();
		assert(_renderer._matrix_stack.back().second == RenderMatrixType::View);
		_renderer._matrix_stack.pop_back();
		assert(_renderer._matrix_stack.back().second == RenderMatrixType::Projection);
		_renderer._matrix_stack.pop_back();
		if (_renderer._matrix_stack.empty())
			return;
		assert(_renderer._matrix_stack.back().second == RenderMatrixType::Model);
#ifndef NDEBUG
		const auto last_view = std::find_if(_renderer._matrix_stack.rbegin(), _renderer._matrix_stack.rend(), [](const auto& m){ return m.second == RenderMatrixType::View; });
		assert(last_view != _renderer._matrix_stack.rend());
		const auto last_projection = std::next(last_view);
		assert(last_projection != _renderer._matrix_stack.rend());
		assert(last_projection->second == RenderMatrixType::Projection);
#endif
	}

	void RenderContextImpl::pop_texture(Flags<Texture2D::Filter> filter) noexcept
	{
		assert(_renderer._texture_stack.size() > 1 || (_renderer._texture_stack.size() == 1 && _renderer._texture_stack.back().second > 1));
		if (_renderer._texture_stack.back().second == 1)
		{
			flush_2d();
			_renderer._texture_stack.pop_back();
			_reset_texture = true;
			reset_texture_state();
		}
		else
			--_renderer._texture_stack.back().second;
		_current_texture_filter = filter;
	}

	void RenderContextImpl::pop_transformation() noexcept
	{
		flush_2d();
		assert(_renderer._matrix_stack.size() > 3);
		assert(_renderer._matrix_stack.back().second == RenderMatrixType::Model);
		_renderer._matrix_stack.pop_back();
		assert(_renderer._matrix_stack.back().second == RenderMatrixType::Model);
	}

	void RenderContextImpl::push_program(const GpuProgram* program)
	{
		assert(!_renderer._program_stack.empty());
		if (_renderer._program_stack.back().first == program)
		{
			++_renderer._program_stack.back().second;
			return;
		}
		flush_2d();
		_renderer._program_stack.emplace_back(program, 1);
		_reset_program = true;
	}

	void RenderContextImpl::push_projection_2d(const Matrix4& matrix)
	{
		flush_2d();
		_renderer._matrix_stack.emplace_back(matrix, RenderMatrixType::Projection);
		_renderer._matrix_stack.emplace_back(Matrix4::identity(), RenderMatrixType::View);
		_renderer._matrix_stack.emplace_back(Matrix4::identity(), RenderMatrixType::Model);
	}

	void RenderContextImpl::push_projection_3d(const Matrix4& projection, const Matrix4& view)
	{
		flush_2d();
		_renderer._matrix_stack.emplace_back(projection, RenderMatrixType::Projection);
		_renderer._matrix_stack.emplace_back(::_3d_directions * view, RenderMatrixType::View);
		_renderer._matrix_stack.emplace_back(Matrix4::identity(), RenderMatrixType::Model);
	}

	Flags<Texture2D::Filter> RenderContextImpl::push_texture(const Texture2D* texture, Flags<Texture2D::Filter> filter)
	{
		if (!texture)
		{
			texture = _renderer._white_texture.get();
			filter = Texture2D::NearestFilter;
		}
		assert(!_renderer._texture_stack.empty());
		if (_renderer._texture_stack.back().first != texture)
		{
			flush_2d();
			_renderer._texture_stack.emplace_back(texture, 1);
			_reset_texture = true;
			reset_texture_state();
		}
		else
			++_renderer._texture_stack.back().second;
		return std::exchange(_current_texture_filter, filter);
	}

	void RenderContextImpl::push_transformation(const Matrix4& matrix)
	{
		assert(!_renderer._matrix_stack.empty());
		assert(_renderer._matrix_stack.back().second == RenderMatrixType::Model);
		_renderer._matrix_stack.emplace_back(_renderer._matrix_stack.back().first * matrix, RenderMatrixType::Model);
	}

	const BackendTexture2D* RenderContextImpl::current_texture_2d() const
	{
		return static_cast<const BackendTexture2D*>(_renderer._texture_stack.back().first);
	}

	void RenderContextImpl::draw_rect(const RectF& position, const Color4f& color, const RectF& texture, const MarginsF& borders)
	{
		Draw2D draw{_renderer._vertices_2d, _renderer._indices_2d};

		RendererBackend::Vertex2D vertex;
		vertex.color = color;

		float left_offset = 0;
		float right_offset = 0;

		// Outer top vertex row.

		vertex.position.y = position.top();
		vertex.texture.y = texture.top();

		vertex.position.x = position.left();
		vertex.texture.x = texture.left();
		draw._vertices << vertex;

		if (borders._left > 0)
		{
			left_offset = borders._left * static_cast<float>(current_texture_2d()->size()._width);

			vertex.position.x = position.left() + left_offset;
			vertex.texture.x = texture.left() + borders._left;
			draw._vertices << vertex;
		}

		if (borders._right > 0)
		{
			right_offset = borders._right * static_cast<float>(current_texture_2d()->size()._width);

			vertex.position.x = position.right() - right_offset;
			vertex.texture.x = texture.right() - borders._right;
			draw._vertices << vertex;
		}

		vertex.position.x = position.right();
		vertex.texture.x = texture.right();
		draw._vertices << vertex;

		// Top/only part indices.

		const auto row_vertices = static_cast<uint16_t>(draw._vertices.count() - draw._index);
		for (uint16_t i = 0; i < row_vertices; ++i)
			draw._indices << static_cast<uint16_t>(draw._index + i) << static_cast<uint16_t>(draw._index + i + row_vertices);

		if (borders._top > 0)
		{
			float top_offset = borders._top * static_cast<float>(current_texture_2d()->size()._height);

			// Inner top vertex row.

			vertex.position.y = position.top() + top_offset;
			vertex.texture.y = texture.top() + borders._top;

			vertex.position.x = position.left();
			vertex.texture.x = texture.left();
			draw._vertices << vertex;

			if (borders._left > 0)
			{
				vertex.position.x = position.left() + left_offset;
				vertex.texture.x = texture.left() + borders._left;
				draw._vertices << vertex;
			}

			if (borders._right > 0)
			{
				vertex.position.x = position.right() - right_offset;
				vertex.texture.x = texture.right() - borders._right;
				draw._vertices << vertex;
			}

			vertex.position.x = position.right();
			vertex.texture.x = texture.right();
			draw._vertices << vertex;

			// Middle/bottom part indices.

			draw._index = static_cast<uint16_t>(draw._index + row_vertices);
			draw._indices << static_cast<uint16_t>(draw._index + row_vertices - 1) << (draw._index);
			for (uint16_t i = 0; i < row_vertices; ++i)
				draw._indices << static_cast<uint16_t>(draw._index + i) << static_cast<uint16_t>(draw._index + i + row_vertices);
		}

		if (borders._bottom > 0)
		{
			float bottom_offset = borders._bottom * static_cast<float>(current_texture_2d()->size()._height);

			// Inner bottom vertex row.

			vertex.position.y = position.bottom() - bottom_offset;
			vertex.texture.y = texture.bottom() - borders._bottom;

			vertex.position.x = position.left();
			vertex.texture.x = texture.left();
			draw._vertices << vertex;

			if (borders._left > 0)
			{
				vertex.position.x = position.left() + left_offset;
				vertex.texture.x = texture.left() + borders._left;
				draw._vertices << vertex;
			}

			if (borders._right > 0)
			{
				vertex.position.x = position.right() - right_offset;
				vertex.texture.x = texture.right() - borders._right;
				draw._vertices << vertex;
			}

			vertex.position.x = position.right();
			vertex.texture.x = texture.right();
			draw._vertices << vertex;

			// Bottom part indices.

			draw._index = static_cast<uint16_t>(draw._index + row_vertices);
			draw._indices << static_cast<uint16_t>(draw._index + row_vertices - 1) << draw._index;
			for (uint16_t i = 0; i < row_vertices; ++i)
				draw._indices << static_cast<uint16_t>(draw._index + i) << static_cast<uint16_t>(draw._index + i + row_vertices);
		}

		// Outer bottom vertex row.

		vertex.position.y = position.bottom();
		vertex.texture.y = texture.bottom();

		vertex.position.x = position.left();
		vertex.texture.x = texture.left();
		draw._vertices << vertex;

		if (borders._left > 0)
		{
			vertex.position.x = position.left() + left_offset;
			vertex.texture.x = texture.left() + borders._left;
			draw._vertices << vertex;
		}

		if (borders._right > 0)
		{
			vertex.position.x = position.right() - right_offset;
			vertex.texture.x = texture.right() - borders._right;
			draw._vertices << vertex;
		}

		vertex.position.x = position.right();
		vertex.texture.x = texture.right();
		draw._vertices << vertex;
	}

	void RenderContextImpl::flush_2d()
	{
		if (_renderer._vertices_2d.size() == 0)
			return;

		if (_renderer._vertices_2d.size() / sizeof(RendererBackend::Vertex2D) > std::numeric_limits<uint16_t>::max())
			throw std::runtime_error("2D vertex buffer size exceeds 16-bit indexing limitations");

		_renderer._program_2d->set_uniform("mvp", full_matrix());
		update_state();
		_renderer._backend->flush_2d(_renderer._vertices_2d, _renderer._indices_2d);
		_statistics._triangles += _renderer._indices_2d.size() / sizeof(uint16_t) - 2;
		++_statistics._draw_calls;
		_renderer._vertices_2d.resize(0);
		_renderer._indices_2d.resize(0);
	}

	void RenderContextImpl::reset_texture_state()
	{
		const auto* texture = current_texture_2d();
		_texture_rect = texture ? texture->full_rectangle() : RectF{};
		_texture_borders = {};
	}

	void RenderContextImpl::update_state()
	{
		if (_reset_program)
		{
			_reset_program = false;
			const auto program = _renderer._program_stack.back().first;
			if (program != _current_program)
			{
				_current_program = program;
				_renderer._backend->set_program(program);
				++_statistics._shader_switches;
#ifndef NDEBUG
				const auto i = std::find(_renderer._seen_programs.begin(), _renderer._seen_programs.end(), program);
				if (i == _renderer._seen_programs.end())
					_renderer._seen_programs.emplace_back(program);
				else
					++_statistics._redundant_shader_switches;
#endif
			}
		}

		if (_reset_texture)
		{
			_reset_texture = false;
			const auto texture = _renderer._texture_stack.back().first;
			if (texture != _current_texture)
			{
				_current_texture = texture;
				_renderer._backend->set_texture(*texture, _current_texture_filter);
				++_statistics._texture_switches;
#ifndef NDEBUG
				const auto i = std::find(_renderer._seen_textures.begin(), _renderer._seen_textures.end(), texture);
				if (i == _renderer._seen_textures.end())
					_renderer._seen_textures.emplace_back(texture);
				else
					++_statistics._redundant_texture_switches;
#endif
			}
		}
	}
}
