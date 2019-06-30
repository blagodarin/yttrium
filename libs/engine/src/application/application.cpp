//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <yttrium/application.h>
#include "application.h"

#include "window.h"

#include <stdexcept>

namespace Yttrium
{
	void ApplicationPrivate::add_window(Application& application, WindowPrivate& window)
	{
		WindowPrivate* expected = nullptr;
		if (!application._private->_window.compare_exchange_strong(expected, &window))
			throw std::logic_error{ "An Application can't have multiple Windows" };
	}

	Application::Application()
		: _private{ std::make_unique<ApplicationPrivate>() }
	{
	}

	Application::~Application() noexcept = default;

	void Application::on_update(const std::function<void(const UpdateEvent&)>& callback)
	{
		_private->_on_update = callback;
	}

	void Application::run()
	{
		using namespace std::literals::chrono_literals;

		auto* window = _private->window();
		if (!window)
			return;

		UpdateEvent update;
		int frames = 0;
		auto max_frame_time = 0ms;
		auto clock = std::chrono::high_resolution_clock::now();
		auto fps_time = 0ms;
		while (window->process_events())
		{
			window->update();

			if (_private->_on_update)
				_private->_on_update(update);

			update.triangles = 0;
			update.draw_calls = 0;
			update.texture_switches = 0;
			update.redundant_texture_switches = 0;
			update.shader_switches = 0;
			update.redundant_shader_switches = 0;

			window->render(update);

			++frames;
			update.milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - clock);
			clock += update.milliseconds;
			fps_time += update.milliseconds;
			if (max_frame_time < update.milliseconds)
				max_frame_time = update.milliseconds;

			if (fps_time >= 1s)
			{
				update.fps = static_cast<int>(frames * 1000 / fps_time.count());
				update.max_frame_time = max_frame_time;
				fps_time = 0ms;
				frames = 0;
				max_frame_time = 0ms;
			}
		}
	}
}
