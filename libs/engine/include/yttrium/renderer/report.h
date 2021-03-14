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

	class RenderClock
	{
	public:
		RenderReport& next_report() noexcept { return _next_report; }
		auto last_frame_duration() noexcept { return _advance; }
		const RenderReport& last_report() const noexcept { return _last_report; }

		void advance() noexcept
		{
			_advance = Clock::now() - _clock;
			_clock += _advance;
			++_frames;
			_period += _advance;
			if (_max_advance < _advance)
				_max_advance = _advance;
			if (_period >= std::chrono::seconds{ 1 })
			{
				_last_report._fps = static_cast<unsigned>((_frames * Clock::period::den + _period.count() - 1) / _period.count());
				_last_report._max_frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(_max_advance);
				update_report(_last_report, _next_report, _frames);
				_next_report = {};
				_frames = 0;
				_max_advance = Clock::duration::zero();
				_period = Clock::duration::zero();
			}
		}

	private:
		void update_report(RenderReport& per_frame, const RenderReport& per_second, unsigned frames) noexcept
		{
			per_frame._triangles = (per_second._triangles + frames - 1) / frames;
			per_frame._draw_calls = (per_second._draw_calls + frames - 1) / frames;
			per_frame._texture_switches = (per_second._texture_switches + frames - 1) / frames;
			per_frame._extra_texture_switches = (per_second._extra_texture_switches + frames - 1) / frames;
			per_frame._shader_switches = (per_second._shader_switches + frames - 1) / frames;
			per_frame._extra_shader_switches = (per_second._extra_shader_switches + frames - 1) / frames;
		}

	private:
		using Clock = std::chrono::high_resolution_clock;
		RenderReport _last_report;
		RenderReport _next_report;
		unsigned _frames = 0;
		Clock::duration _max_advance{ 0 };
		Clock::duration _period{ 0 };
		Clock::duration _advance{ 0 };
		Clock::time_point _clock = Clock::now();
	};
}
