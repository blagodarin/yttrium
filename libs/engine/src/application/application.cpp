// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/application/application.h>
#include "application.h"

#include <yttrium/renderer/report.h>
#include "window.h"

#include <stdexcept>

namespace
{
	void update_report(Yt::RenderReport& per_frame, const Yt::RenderReport& per_second, unsigned frames)
	{
		per_frame._triangles = (per_second._triangles + frames - 1) / frames;
		per_frame._draw_calls = (per_second._draw_calls + frames - 1) / frames;
		per_frame._texture_switches = (per_second._texture_switches + frames - 1) / frames;
		per_frame._extra_texture_switches = (per_second._extra_texture_switches + frames - 1) / frames;
		per_frame._shader_switches = (per_second._shader_switches + frames - 1) / frames;
		per_frame._extra_shader_switches = (per_second._extra_shader_switches + frames - 1) / frames;
	}
}

namespace Yt
{
	bool ApplicationPrivate::process_events()
	{
		if (auto* window = _window.load(); window && window->process_events())
		{
			window->update();
			return true;
		}
		return false;
	}

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

	void Application::run(const std::function<void(const std::chrono::milliseconds&)>& idle_callback)
	{
		using namespace std::literals::chrono_literals;

		RenderReport last_report;
		RenderReport next_report;

		auto advance = 0ms;
		unsigned frames = 0;
		auto max_frame_time = 0ms;
		auto clock = std::chrono::high_resolution_clock::now();
		auto fps_time = 0ms;
		while (_private->process_events())
		{
			if (idle_callback)
				idle_callback(advance);

			_private->window()->render(next_report, last_report);

			++frames;
			advance = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - clock);
			clock += advance;
			fps_time += advance;
			if (max_frame_time < advance)
				max_frame_time = advance;

			if (fps_time >= 1s)
			{
				const auto milliseconds = static_cast<unsigned>(fps_time.count());
				last_report._fps = (frames * 1000 + milliseconds - 1) / milliseconds;
				last_report._max_frame_time = max_frame_time;
				::update_report(last_report, next_report, frames);
				next_report = {};
				fps_time = 0ms;
				frames = 0;
				max_frame_time = 0ms;
			}
		}
	}
}
