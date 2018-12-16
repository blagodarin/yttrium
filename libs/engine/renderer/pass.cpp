//
// Copyright 2018 Sergei Blagodarin
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

#include "pass.h"

#include <yttrium/math/line.h>
#include <yttrium/math/matrix.h>
#include <yttrium/math/quad.h>
#include <yttrium/memory/buffer_appender.h>
#include <yttrium/renderer/program.h>
#include <yttrium/renderer/textured_rect.h>
#include <yttrium/utils/string.h>
#include "backend.h"
#include "builtin/builtin.h"
#include "debug_renderer.h"
#include "texture.h"

#include <cassert>

namespace
{
	const Yttrium::Matrix4 _3d_directions // Makes Y point forward and Z point up.
		{
			1,
			0,
			0,
			0,
			0,
			0,
			1,
			0,
			0,
			-1,
			0,
			0,
			0,
			0,
			0,
			1,
		};
}

namespace Yttrium
{
	struct RenderPassImpl::Batch2D
	{
		RenderBackend::Vertex2D* _vertices = nullptr;
		std::uint16_t* _indices = nullptr;
		std::size_t _base_index = 0;
	};

	RenderPassData::RenderPassData() = default;

	RenderPassData::~RenderPassData() noexcept = default;

	RenderPassImpl::RenderPassImpl(RenderBackend& backend, RenderBuiltin& builtin, RenderPassData& data, const Size& window_size)
		: _backend{ backend }
		, _builtin{ builtin }
		, _data{ data }
		, _window_size{ window_size }
	{
		_data._debug_text.clear();
		_backend.clear();
	}

	RenderPassImpl::~RenderPassImpl() noexcept
	{
#ifndef NDEBUG
		_data._seen_textures.clear();
		_data._seen_programs.clear();
#endif
	}

	void RenderPassImpl::add_debug_text(std::string_view text)
	{
		append_to(_data._debug_text, text, '\n');
	}

	void RenderPassImpl::draw_mesh(const Mesh& mesh)
	{
		update_state();
		_statistics._triangles += _backend.draw_mesh(mesh);
		++_statistics._draw_calls;
	}

	void RenderPassImpl::draw_quad(const Quad& quad, const Color4f& color)
	{
		const auto batch = prepare_batch_2d(4, 4);

		batch._vertices[0] = { quad._a, color, _texture_rect.top_left() };
		batch._vertices[1] = { quad._d, color, _texture_rect.bottom_left() };
		batch._vertices[2] = { quad._b, color, _texture_rect.top_right() };
		batch._vertices[3] = { quad._c, color, _texture_rect.bottom_right() };

		batch._indices[0] = static_cast<std::uint16_t>(batch._base_index);
		batch._indices[1] = static_cast<std::uint16_t>(batch._base_index + 1);
		batch._indices[2] = static_cast<std::uint16_t>(batch._base_index + 2);
		batch._indices[3] = static_cast<std::uint16_t>(batch._base_index + 3);
	}

	void RenderPassImpl::draw_rect(const RectF& rect, const Color4f& color)
	{
		draw_rect(rect, color, _texture_rect, _texture_borders);
	}

	void RenderPassImpl::draw_rects(const std::vector<TexturedRect>& rects, const Color4f& color)
	{
		const SizeF texture_size{ current_texture_2d()->size() };
		const Vector2 texture_scale{ texture_size._width, texture_size._height };
		for (const auto& rect : rects)
			draw_rect(rect.geometry, color, _backend.map_rect(rect.texture / texture_scale, current_texture_2d()->orientation()));
	}

	Matrix4 RenderPassImpl::full_matrix() const
	{
		const auto current_projection = std::find_if(_data._matrix_stack.rbegin(), _data._matrix_stack.rend(), [](const auto& m) { return m.second == RenderMatrixType::Projection; });
		assert(current_projection != _data._matrix_stack.rend());
		const auto current_view = current_projection.base();
		assert(current_view != _data._matrix_stack.end());
		assert(current_view->second == RenderMatrixType::View);
		return current_projection->first * current_view->first * model_matrix();
	}

	Matrix4 RenderPassImpl::model_matrix() const
	{
		assert(!_data._matrix_stack.empty());
		assert(_data._matrix_stack.back().second == RenderMatrixType::Model);
		return _data._matrix_stack.back().first;
	}

	Line3 RenderPassImpl::pixel_ray(const Vector2& v) const
	{
		// Move each coordinate to the center of the pixel (by adding 0.5), then normalize from [0, D] to [-1, 1].
		const auto xn = (2 * v.x + 1) / static_cast<float>(_window_size._width) - 1;
		const auto yn = 1 - (2 * v.y + 1) / static_cast<float>(_window_size._height);
		const auto m = inverse(full_matrix());
		return { m * Vector3{ xn, yn, 0 }, m * Vector3{ xn, yn, 1 } };
	}

	void RenderPassImpl::set_texture_rect(const RectF& rect, const MarginsF& borders)
	{
		const auto current_texture = current_texture_2d();
		if (!current_texture)
			return;

		const SizeF texture_size{ current_texture->size() };
		const Vector2 texture_scale{ texture_size._width, texture_size._height };
		const auto texture_rect_size = _texture_rect.size();
		const auto minimum = SizeF{ borders._left + 1 + borders._right, borders._top + 1 + borders._bottom } / texture_scale;
		if (texture_rect_size._width < minimum._width || texture_rect_size._height < minimum._height)
			return;

		_texture_rect = _backend.map_rect(rect / texture_scale, current_texture->orientation());
		_texture_borders =
			{
				borders._top / texture_size._height,
				borders._right / texture_size._width,
				borders._bottom / texture_size._height,
				borders._left / texture_size._width,
			};
	}

	SizeF RenderPassImpl::window_size() const
	{
		return SizeF{ _window_size };
	}

	void RenderPassImpl::draw_debug_text()
	{
		if (_data._debug_text.empty())
			return;
		{
			DebugRenderer debug{ *this };
			debug.set_color(1, 1, 1);
			size_t top = 0;
			size_t line_begin = 0;
			auto line_end = _data._debug_text.find('\n', line_begin);
			while (line_end != std::string::npos)
			{
				debug.draw_text(0, top++, { _data._debug_text.data() + line_begin, line_end - line_begin });
				line_begin = line_end + 1;
				line_end = _data._debug_text.find('\n', line_begin);
			}
			debug.draw_text(0, top, { _data._debug_text.data() + line_begin, _data._debug_text.size() - line_begin });
		}
		flush_2d();
	}

	void RenderPassImpl::draw_rect(const RectF& position, const Color4f& color, const RectF& texture)
	{
		auto batch = prepare_batch_2d(4, 4);

		batch._vertices[0] = { position.top_left(), color, texture.top_left() };
		batch._vertices[1] = { position.bottom_left(), color, texture.bottom_left() };
		batch._vertices[2] = { position.top_right(), color, texture.top_right() };
		batch._vertices[3] = { position.bottom_right(), color, texture.bottom_right() };

		batch._indices[0] = static_cast<std::uint16_t>(batch._base_index);
		batch._indices[1] = static_cast<std::uint16_t>(batch._base_index + 1);
		batch._indices[2] = static_cast<std::uint16_t>(batch._base_index + 2);
		batch._indices[3] = static_cast<std::uint16_t>(batch._base_index + 3);
	}

	void RenderPassImpl::pop_program() noexcept
	{
		assert(_data._program_stack.size() > 1 || (_data._program_stack.size() == 1 && _data._program_stack.back().second > 1));
		if (_data._program_stack.back().second > 1)
		{
			--_data._program_stack.back().second;
			return;
		}
		flush_2d();
		_data._program_stack.pop_back();
		_reset_program = true;
	}

	void RenderPassImpl::pop_projection() noexcept
	{
		flush_2d();
		assert(_data._matrix_stack.size() >= 3);
		assert(_data._matrix_stack.back().second == RenderMatrixType::Model);
		_data._matrix_stack.pop_back();
		assert(_data._matrix_stack.back().second == RenderMatrixType::View);
		_data._matrix_stack.pop_back();
		assert(_data._matrix_stack.back().second == RenderMatrixType::Projection);
		_data._matrix_stack.pop_back();
		if (_data._matrix_stack.empty())
			return;
		assert(_data._matrix_stack.back().second == RenderMatrixType::Model);
#ifndef NDEBUG
		const auto last_view = std::find_if(_data._matrix_stack.rbegin(), _data._matrix_stack.rend(), [](const auto& m) { return m.second == RenderMatrixType::View; });
		assert(last_view != _data._matrix_stack.rend());
		const auto last_projection = std::next(last_view);
		assert(last_projection != _data._matrix_stack.rend());
		assert(last_projection->second == RenderMatrixType::Projection);
#endif
	}

	void RenderPassImpl::pop_texture(Flags<Texture2D::Filter> filter) noexcept
	{
		assert(_data._texture_stack.size() > 1 || (_data._texture_stack.size() == 1 && _data._texture_stack.back().second > 1));
		if (_data._texture_stack.back().second == 1)
		{
			flush_2d();
			_data._texture_stack.pop_back();
			_reset_texture = true;
			reset_texture_state();
		}
		else
			--_data._texture_stack.back().second;
		_current_texture_filter = filter;
	}

	void RenderPassImpl::pop_transformation() noexcept
	{
		flush_2d();
		assert(_data._matrix_stack.size() > 3);
		assert(_data._matrix_stack.back().second == RenderMatrixType::Model);
		_data._matrix_stack.pop_back();
		assert(_data._matrix_stack.back().second == RenderMatrixType::Model);
	}

	void RenderPassImpl::push_program(const RenderProgram* program)
	{
		assert(!_data._program_stack.empty());
		if (_data._program_stack.back().first == program)
		{
			++_data._program_stack.back().second;
			return;
		}
		flush_2d();
		_data._program_stack.emplace_back(program, 1);
		_reset_program = true;
	}

	void RenderPassImpl::push_projection_2d(const Matrix4& matrix)
	{
		flush_2d();
		_data._matrix_stack.emplace_back(matrix, RenderMatrixType::Projection);
		_data._matrix_stack.emplace_back(Matrix4::identity(), RenderMatrixType::View);
		_data._matrix_stack.emplace_back(Matrix4::identity(), RenderMatrixType::Model);
	}

	void RenderPassImpl::push_projection_3d(const Matrix4& projection, const Matrix4& view)
	{
		flush_2d();
		_data._matrix_stack.emplace_back(projection, RenderMatrixType::Projection);
		_data._matrix_stack.emplace_back(::_3d_directions * view, RenderMatrixType::View);
		_data._matrix_stack.emplace_back(Matrix4::identity(), RenderMatrixType::Model);
	}

	Flags<Texture2D::Filter> RenderPassImpl::push_texture(const Texture2D* texture, Flags<Texture2D::Filter> filter)
	{
		if (!texture)
		{
			texture = _builtin._white_texture.get();
			filter = Texture2D::NearestFilter;
		}
		assert(!_data._texture_stack.empty());
		if (_data._texture_stack.back().first != texture)
		{
			flush_2d();
			_data._texture_stack.emplace_back(texture, 1);
			_reset_texture = true;
			reset_texture_state();
		}
		else
			++_data._texture_stack.back().second;
		return std::exchange(_current_texture_filter, filter);
	}

	void RenderPassImpl::push_transformation(const Matrix4& matrix)
	{
		assert(!_data._matrix_stack.empty());
		assert(_data._matrix_stack.back().second == RenderMatrixType::Model);
		_data._matrix_stack.emplace_back(_data._matrix_stack.back().first * matrix, RenderMatrixType::Model);
	}

	const BackendTexture2D* RenderPassImpl::current_texture_2d() const
	{
		return static_cast<const BackendTexture2D*>(_data._texture_stack.back().first);
	}

	void RenderPassImpl::draw_rect(const RectF& position, const Color4f& color, const RectF& texture, const MarginsF& borders)
	{
		const auto px0 = position.left();
		const auto px1 = position.left() + borders._left * static_cast<float>(current_texture_2d()->size()._width);
		const auto px2 = position.right() - borders._right * static_cast<float>(current_texture_2d()->size()._width);
		const auto px3 = position.right();

		const auto py0 = position.top();
		const auto py1 = position.top() + borders._top * static_cast<float>(current_texture_2d()->size()._height);
		const auto py2 = position.bottom() - borders._bottom * static_cast<float>(current_texture_2d()->size()._height);
		const auto py3 = position.bottom();

		const bool has_left_border = px0 != px1;
		const bool has_right_border = px2 != px3;
		const bool has_top_border = py0 != py1;
		const bool has_bottom_border = py2 != py3;

		const auto row_vertices = 2 + static_cast<std::size_t>(has_left_border) + static_cast<std::size_t>(has_right_border);
		const auto stripe_count = 1 + static_cast<std::size_t>(has_top_border) + static_cast<std::size_t>(has_bottom_border);
		const auto vertex_count = row_vertices * (stripe_count + 1);
		const auto index_count = 2 * (stripe_count * row_vertices + stripe_count - 1);

		auto batch = prepare_batch_2d(vertex_count, index_count);

		const auto tx0 = texture.left();
		const auto tx1 = texture.left() + borders._left;
		const auto tx2 = texture.right() - borders._right;
		const auto tx3 = texture.right();

		const auto ty0 = texture.top();
		const auto ty3 = texture.bottom();

		*batch._vertices++ = { { px0, py0 }, color, { tx0, ty0 } };
		if (has_left_border)
			*batch._vertices++ = { { px1, py0 }, color, { tx1, ty0 } };
		if (has_right_border)
			*batch._vertices++ = { { px2, py0 }, color, { tx2, ty0 } };
		*batch._vertices++ = { { px3, py0 }, color, { tx3, ty0 } };
		if (has_top_border)
		{
			const auto ty1 = texture.top() + borders._top;
			*batch._vertices++ = { { px0, py1 }, color, { tx0, ty1 } };
			if (has_left_border)
				*batch._vertices++ = { { px1, py1 }, color, { tx1, ty1 } };
			if (has_right_border)
				*batch._vertices++ = { { px2, py1 }, color, { tx2, ty1 } };
			*batch._vertices++ = { { px3, py1 }, color, { tx3, ty1 } };
		}
		if (has_bottom_border)
		{
			const auto ty2 = texture.bottom() - borders._bottom;
			*batch._vertices++ = { { px0, py2 }, color, { tx0, ty2 } };
			if (has_left_border)
				*batch._vertices++ = { { px1, py2 }, color, { tx1, ty2 } };
			if (has_right_border)
				*batch._vertices++ = { { px2, py2 }, color, { tx2, ty2 } };
			*batch._vertices++ = { { px3, py2 }, color, { tx3, ty2 } };
		}
		*batch._vertices++ = { { px0, py3 }, color, { tx0, ty3 } };
		if (has_left_border)
			*batch._vertices++ = { { px1, py3 }, color, { tx1, ty3 } };
		if (has_right_border)
			*batch._vertices++ = { { px2, py3 }, color, { tx2, ty3 } };
		*batch._vertices = { { px3, py3 }, color, { tx3, ty3 } };

		for (std::size_t i = 0; i < stripe_count; ++i)
		{
			if (i > 0)
			{
				*batch._indices++ = static_cast<std::uint16_t>(batch._base_index + row_vertices - 1);
				*batch._indices++ = static_cast<std::uint16_t>(batch._base_index);
			}
			for (std::size_t j = 0; j < row_vertices; ++j)
			{
				*batch._indices++ = static_cast<std::uint16_t>(batch._base_index + j);
				*batch._indices++ = static_cast<std::uint16_t>(batch._base_index + j + row_vertices);
			}
			batch._base_index += row_vertices;
		}
	}

	void RenderPassImpl::flush_2d() noexcept
	{
		assert(_data._vertices_2d.size() / sizeof(RenderBackend::Vertex2D) <= std::size_t{ std::numeric_limits<uint16_t>::max() } + 1);
		if (_data._vertices_2d.size() == 0)
			return;

		_builtin._program_2d->set_uniform("mvp", full_matrix());
		update_state();
		_backend.flush_2d(_data._vertices_2d, _data._indices_2d);
		_statistics._triangles += _data._indices_2d.size() / sizeof(uint16_t) - 2;
		++_statistics._draw_calls;
		_data._vertices_2d.resize(0);
		_data._indices_2d.resize(0);
	}

	void RenderPassImpl::reset_texture_state()
	{
		const auto* texture = current_texture_2d();
		_texture_rect = texture ? texture->full_rectangle() : RectF{};
		_texture_borders = {};
	}

	RenderPassImpl::Batch2D RenderPassImpl::prepare_batch_2d(std::size_t vertex_count, std::size_t index_count)
	{
		auto next_index = _data._vertices_2d.size() / sizeof(RenderBackend::Vertex2D);
		if (next_index > std::numeric_limits<std::uint16_t>::max() - vertex_count)
		{
			flush_2d();
			next_index = 0;
		}
		const auto vertex_buffer_size = _data._vertices_2d.size() + sizeof(RenderBackend::Vertex2D) * vertex_count;
		const auto index_buffer_size = _data._indices_2d.size() + sizeof(std::uint16_t) * (index_count + (next_index > 0 ? 2 : 0));
		_data._vertices_2d.reserve(vertex_buffer_size);
		_data._indices_2d.reserve(index_buffer_size);
		auto* const vertices = reinterpret_cast<RenderBackend::Vertex2D*>(_data._vertices_2d.end());
		auto* indices = reinterpret_cast<std::uint16_t*>(_data._indices_2d.end());
		_data._vertices_2d.resize(vertex_buffer_size);
		_data._indices_2d.resize(index_buffer_size);
		if (next_index > 0)
		{
			*indices++ = static_cast<std::uint16_t>(next_index - 1);
			*indices++ = static_cast<std::uint16_t>(next_index);
		}
		return { vertices, indices, next_index };
	}

	void RenderPassImpl::update_state()
	{
		if (_reset_program)
		{
			_reset_program = false;
			const auto program = _data._program_stack.back().first;
			if (program != _current_program)
			{
				_current_program = program;
				_backend.set_program(program);
				++_statistics._shader_switches;
#ifndef NDEBUG
				const auto i = std::find(_data._seen_programs.begin(), _data._seen_programs.end(), program);
				if (i == _data._seen_programs.end())
					_data._seen_programs.emplace_back(program);
				else
					++_statistics._redundant_shader_switches;
#endif
			}
		}

		if (_reset_texture)
		{
			_reset_texture = false;
			const auto texture = _data._texture_stack.back().first;
			if (texture != _current_texture)
			{
				_current_texture = texture;
				_backend.set_texture(*texture, _current_texture_filter);
				++_statistics._texture_switches;
#ifndef NDEBUG
				const auto i = std::find(_data._seen_textures.begin(), _data._seen_textures.end(), texture);
				if (i == _data._seen_textures.end())
					_data._seen_textures.emplace_back(texture);
				else
					++_statistics._redundant_texture_switches;
#endif
			}
		}
	}
}
