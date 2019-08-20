//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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
