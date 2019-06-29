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

#pragma once

#include <yttrium/api.h>

#include <chrono>
#include <functional>
#include <memory>
#include <optional>

namespace Yttrium
{
	///
	struct UpdateEvent
	{
		/// Milliseconds to update the time by.
		std::chrono::milliseconds milliseconds{ 0 };

		/// Current updates per second (frames per second, FPS) rate.
		std::optional<int> fps;

		/// Maximum frame time in milliseconds.
		std::chrono::milliseconds max_frame_time{ 0 };

		/// Number of triangles rendered.
		size_t triangles = 0;

		/// Number of draw calls issued.
		size_t draw_calls = 0;

		/// Number of texture switches.
		size_t texture_switches = 0;

		/// Number of redundant texture switches, i.e. those which set a texture that had already been set in that frame.
		/// \note Works only in debug mode.
		size_t redundant_texture_switches = 0;

		/// Number of shader switches.
		size_t shader_switches = 0;

		/// Number of redundant shader switches, i.e. those which set a GPU program that had already been set in that frame.
		/// \note Works only in debug mode.
		size_t redundant_shader_switches = 0;
	};

	///
	class Y_ENGINE_API Application
	{
	public:
		Application();
		~Application() noexcept;

		///
		void on_update(const std::function<void(const UpdateEvent&)>&);

		///
		void run();

	private:
		const std::unique_ptr<class ApplicationPrivate> _private;
		friend ApplicationPrivate;
	};
}
