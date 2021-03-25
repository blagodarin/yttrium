// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>

namespace Yt
{
	class RenderMetrics
	{
	public:
		size_t _triangles = 0;              // Triangles per frame.
		size_t _draw_calls = 0;             // Draw calls per frame.
		size_t _texture_switches = 0;       // Texture switches per frame.
		size_t _extra_texture_switches = 0; // Switches to textures already used for the frame (debug only).
		size_t _shader_switches = 0;        // Shader switches per frame.
		size_t _extra_shader_switches = 0;  // Switches to shaders already used for the frame (debug only).

		constexpr RenderMetrics& operator+=(const RenderMetrics& other) noexcept
		{
			_triangles += other._triangles;
			_draw_calls += other._draw_calls;
			_texture_switches += other._texture_switches;
			_extra_texture_switches += other._extra_texture_switches;
			_shader_switches += other._shader_switches;
			_extra_shader_switches += other._extra_shader_switches;
			return *this;
		}
	};

	constexpr RenderMetrics operator/(const RenderMetrics& metrics, unsigned frames) noexcept
	{
		return {
			(metrics._triangles + frames - 1) / frames,
			(metrics._draw_calls + frames - 1) / frames,
			(metrics._texture_switches + frames - 1) / frames,
			(metrics._extra_texture_switches + frames - 1) / frames,
			(metrics._shader_switches + frames - 1) / frames,
			(metrics._extra_shader_switches + frames - 1) / frames,
		};
	}
}
