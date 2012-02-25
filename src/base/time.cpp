#include <Yttrium/time.hpp>

namespace Yttrium
{

enum
{
	RateCounterPeriod = 1000, // The rate changes at most once per second.
};

Clock Timer::reset()
{
	Clock result = _time;
	if (_started)
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
	if (_started)
	{
		result += time - _start_time;
		_start_time = time;
	}
	else
	{
		_start_time = time;
		_started = true;
	}
	_time = 0;
	return result;
}

void Timer::start()
{
	if (!_started)
	{
		_start_time = clock();
		_started = true;
	}
}

Clock Timer::stop()
{
	if (_started)
	{
		_time += clock() - _start_time;
		_started = false;
	}
	return _time;
}

void RateCounter::start()
{
	_last_time = Timer::clock();
	_total_time = 0;
	_ticks = 0;
	_rate = 0;
}

void RateCounter::tick()
{
	Clock time = Timer::clock();
	_total_time += time - _last_time;
	++_ticks;
	if (_total_time >= RateCounterPeriod)
	{
		_rate = _ticks * 1000 / _total_time;
		_total_time = 0;
		_ticks = 0;
	}
	_last_time = time;
}

} // namespace Yttrium
