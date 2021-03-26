// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cassert>
#include <chrono>

namespace Yt
{
	class Clock
	{
	public:
		constexpr Clock() noexcept = default;

		int tick() noexcept
		{
			const auto now = std::chrono::steady_clock::now();
			if (_time == std::chrono::steady_clock::time_point{})
			{
				_time = now;
				return 0;
			}
			const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - _time);
			_time += duration;
			return duration.count();
		}

	private:
		std::chrono::steady_clock::time_point _time;
	};

	class FrameClock
	{
	public:
		struct Report
		{
			unsigned _frameCount = 0;      // Number of frames used for the measurement.
			unsigned _framesPerSecond = 0; // Frames per second.
			unsigned _maxFrameTime = 0;    // Maximum duration of a single frame.
		};

		constexpr FrameClock() noexcept = default;

		bool update(Report& report) noexcept
		{
			const auto now = std::chrono::steady_clock::now();
			if (_tickTime != std::chrono::steady_clock::time_point{})
			{
				assert(_measurementTime < std::chrono::seconds{ 1 });
				const auto frameDuration = now - _tickTime;
				_tickTime = now;
				++_frameCount;
				_measurementTime += frameDuration;
				if (frameDuration > _maxFrameTime)
					_maxFrameTime = frameDuration;
				if (_measurementTime >= std::chrono::seconds{ 1 })
				{
					report._frameCount = _frameCount;
					report._framesPerSecond = static_cast<int>((_frameCount * std::chrono::steady_clock::period::den + _measurementTime.count() - 1) / _measurementTime.count());
					report._maxFrameTime = std::chrono::duration_cast<std::chrono::milliseconds>(_maxFrameTime).count();
					_frameCount = 0;
					_measurementTime = std::chrono::steady_clock::duration::zero();
					_maxFrameTime = std::chrono::steady_clock::duration::zero();
					return true;
				}
			}
			else
			{
				_tickTime = now;
				assert(_frameCount == 0);
				assert(_measurementTime == std::chrono::steady_clock::duration::zero());
				assert(_maxFrameTime == std::chrono::steady_clock::duration::zero());
			}
			return false;
		}

	private:
		std::chrono::steady_clock::time_point _tickTime;
		unsigned _frameCount = 0;
		std::chrono::steady_clock::duration _measurementTime{ 0 };
		std::chrono::steady_clock::duration _maxFrameTime{ 0 };
	};
}
