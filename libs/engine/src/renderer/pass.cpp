// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "pass.h"

#include <yttrium/math/line.h>
#include <yttrium/math/matrix.h>
#include <yttrium/math/quad.h>
#include <yttrium/memory/buffer_appender.h>
#include <yttrium/renderer/program.h>
#include <yttrium/renderer/report.h>
#include <yttrium/utils/string.h>
#include "backend.h"
#include "builtin.h"
#include "texture.h"

#include <cassert>

namespace
{
	// Makes Y point forward and Z point up.
	const Yt::Matrix4 _3d_directions{
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, -1, 0, 0,
		0, 0, 0, 1
	};
}

namespace Yt
{
	RenderPassData::RenderPassData() = default;

	RenderPassData::~RenderPassData() noexcept = default;

	RenderPassImpl::RenderPassImpl(RenderBackend& backend, RenderBuiltin& builtin, RenderPassData& data, const Size& viewport_size, RenderReport& report)
		: _backend{ backend }
		, _builtin{ builtin }
		, _data{ data }
		, _viewport_size{ viewport_size }
		, _report{ report }
	{
		_backend.clear();
	}

	RenderPassImpl::~RenderPassImpl() noexcept
	{
#ifndef NDEBUG
		_data._seen_textures.clear();
		_data._seen_programs.clear();
#endif
	}

	void RenderPassImpl::draw_mesh(const Mesh& mesh)
	{
		update_state();
		_report._triangles += _backend.draw_mesh(mesh);
		++_report._draw_calls;
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
		const auto xn = (2 * v.x + 1) / static_cast<float>(_viewport_size._width) - 1;
		const auto yn = 1 - (2 * v.y + 1) / static_cast<float>(_viewport_size._height);
		const auto m = inverse(full_matrix());
		return { m * Vector3{ xn, yn, 0 }, m * Vector3{ xn, yn, 1 } };
	}

	RectF RenderPassImpl::viewport_rect() const
	{
		return RectF{ _viewport_size };
	}

	void RenderPassImpl::pop_program() noexcept
	{
		assert(_data._program_stack.size() > 1 || (_data._program_stack.size() == 1 && _data._program_stack.back().second > 1));
		if (_data._program_stack.back().second > 1)
		{
			--_data._program_stack.back().second;
			return;
		}
		_data._program_stack.pop_back();
		_reset_program = true;
	}

	void RenderPassImpl::pop_projection() noexcept
	{
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
			_data._texture_stack.pop_back();
			_reset_texture = true;
		}
		else
			--_data._texture_stack.back().second;
		_current_texture_filter = filter;
	}

	void RenderPassImpl::pop_transformation() noexcept
	{
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
		_data._program_stack.emplace_back(program, 1);
		_reset_program = true;
	}

	void RenderPassImpl::push_projection_2d(const Matrix4& matrix)
	{
		_data._matrix_stack.emplace_back(matrix, RenderMatrixType::Projection);
		_data._matrix_stack.emplace_back(Matrix4::identity(), RenderMatrixType::View);
		_data._matrix_stack.emplace_back(Matrix4::identity(), RenderMatrixType::Model);
	}

	void RenderPassImpl::push_projection_3d(const Matrix4& projection, const Matrix4& view)
	{
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
			_data._texture_stack.emplace_back(texture, 1);
			_reset_texture = true;
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

	void RenderPassImpl::flush_2d(const Buffer& vertices, const Buffer& indices) noexcept
	{
		update_state();
		_backend.flush_2d(vertices, indices);
		_report._triangles += indices.size() / sizeof(uint16_t) - 2;
		++_report._draw_calls;
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
				++_report._shader_switches;
#ifndef NDEBUG
				if (std::none_of(_data._seen_programs.begin(), _data._seen_programs.end(), [program](const auto seen_program) { return program == seen_program; }))
					_data._seen_programs.emplace_back(program);
				else
					++_report._extra_shader_switches;
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
				++_report._texture_switches;
#ifndef NDEBUG
				if (std::none_of(_data._seen_textures.begin(), _data._seen_textures.end(), [texture](const auto seen_texture) { return texture == seen_texture; }))
					_data._seen_textures.emplace_back(texture);
				else
					++_report._extra_texture_switches;
#endif
			}
		}
	}
}
