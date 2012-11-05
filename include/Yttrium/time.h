/// \file
/// \brief Time-related facilities.

#ifndef __Y_TIME_H
#define __Y_TIME_H

#include <Yttrium/types.h>

namespace Yttrium
{

/// Date and time representation.

struct Y_API DateTime
{
	uint_fast16_t year;   ///< Year.
	uint_fast8_t  month;  ///< Month (1 - 12).
	uint_fast8_t  day;    ///< Day (1 - 31).
	uint_fast8_t  hour;   ///< Hour (0 - 23).
	uint_fast8_t  minute; ///< Minute.
	uint_fast8_t  second; ///< Second.

	/// Get the current date and time.

	static DateTime now() noexcept;
};

/// Integer value representing timer time.
/// \note Passing negative clock values to functions results in undefined behavior.

typedef int_fast32_t Clock;

/// %Timer class.

class Y_API Timer
{
public:

	///

	inline Timer() noexcept;

	/// Return \c true if the timer is started.
	/// \return \c true for a started timer.

	inline bool is_started() const noexcept;

	/// Reset the timer time without changing any other state.
	/// \return Timer value on reset.

	Clock reset() noexcept;

	/// Reset the timer and start it.
	/// \return Timer value on restart.

	Clock restart() noexcept;

	/// Set the stored time to the specified value.
	/// \param time Time to set.

	inline void set_time(Clock time) noexcept;

	/// Start the timer if it's not started.

	void start() noexcept;

	/// Stop the timer.
	/// \return Timer value on stop.

	Clock stop() noexcept;

	/// Get the current timer time.
	/// \return Timer time.

	inline Clock time() const noexcept;

public:

	/// Return the current clock counter value.
	/// \return Clock counter value in milliseconds.
	/// \note Timer functionality is based on this function.

	static Clock clock() noexcept;

private:

	Clock _time;
	bool  _is_started;
	Clock _start_time;
};

/// Rate counter.

class Y_API RateCounter // TODO: Rename.
{
public:

	///

	inline RateCounter() noexcept;

	/// Start the counter.
	/// \note Should be called just before the beginning of the counted loop.

	void start() noexcept;

	/// Mark a tick.
	/// \note Should be called in the very end of the counted loop.

	void tick() noexcept;

	/// Return the current rate.
	/// \return Current rate.

	inline Clock rate() const noexcept;

	// TODO: Add was_changed().

private:

	Clock _last_time;
	Clock _total_time;
	Clock _ticks;
	Clock _rate;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Timer::Timer() noexcept
	: _time(0)
	, _is_started(false)
{
}

bool Timer::is_started() const noexcept
{
	return _is_started;
}

void Timer::set_time(Clock time) noexcept
{
	_time = time;
}

Clock Timer::time() const noexcept
{
	return _is_started
		? _time + (clock() - _start_time)
		: _time;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RateCounter::RateCounter() noexcept
	: _rate(0)
{
}

Clock RateCounter::rate() const noexcept
{
	return _rate;
}

} // namespace Yttrium

#endif // __Y_TIME_H
