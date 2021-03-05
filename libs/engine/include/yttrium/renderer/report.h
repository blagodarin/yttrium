// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <cstdint>

namespace Yt
{
	//!
	struct RenderReport
	{
		uint_fast32_t _fps = 0;                         // Frames per second.
		std::chrono::milliseconds _max_frame_time{ 0 }; // Maximum frame duration.
		uint_fast64_t _triangles = 0;                   // Triangles per frame.
		uint_fast32_t _draw_calls = 0;                  // Draw calls per frame.
		uint_fast32_t _texture_switches = 0;            // Texture switches per frame.
		uint_fast32_t _extra_texture_switches = 0;      // Switches to textures already used for the frame (debug only).
		uint_fast32_t _shader_switches = 0;             // Shader switches per frame.
		uint_fast32_t _extra_shader_switches = 0;       // Switches to shaders already used for the frame (debug only).
	};
}
