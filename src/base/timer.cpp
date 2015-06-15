#include <yttrium/timer.h>

#include <chrono>

namespace Yttrium
{
	Clock Timer::reset()
	{
		Clock result = _time;
		if (_is_started)
		{
			Clock time = clock();
			result += time - _start_time;
			_start_time = time;
		}
		_time = 0;
		return result;
	}

	Clock Timer::restart()
	{
		Clock time = clock();
		Clock result = _time;
		if (_is_started)
		{
			result += time - _start_time;
			_start_time = time;
		}
		else
		{
			_start_time = time;
			_is_started = true;
		}
		_time = 0;
		return result;
	}

	void Timer::start()
	{
		if (!_is_started)
		{
			_start_time = clock();
			_is_started = true;
		}
	}

	Clock Timer::stop()
	{
		if (_is_started)
		{
			_time += clock() - _start_time;
			_is_started = false;
		}
		return _time;
	}

	Clock Timer::clock()
	{
		return std::chrono::high_resolution_clock::is_steady
			? std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()
			: std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}
}
