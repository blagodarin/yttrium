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

	Timer() noexcept
		: _time(0)
		, _started(false)
	{
	}

	/// Reset the timer time without changing any other state.
	/// \return Timer value on reset.

	Clock reset() noexcept;

	/// Reset the timer and start it.
	/// \return Timer value on restart.

	Clock restart() noexcept;

	/// Set the stored time to the specified value.
	/// \param time Time to set.

	void setTime(Clock time) noexcept
	{
		_time = time;
	}

	/// Start the timer if it's not started.

	void start() noexcept;

	/// Stop the timer.
	/// \return Timer value on stop.

	Clock stop() noexcept;

	/// Get the current timer time.
	/// \return Timer time.

	Clock time() const noexcept
	{
		return (_time + (_started ? clock() - _start_time : 0));
	}

	/// Return \c true if the timer is started.
	/// \return \c true for a started timer.

	bool started() const noexcept
	{
		return _started;
	}

public:

	/// Return the current clock counter value.
	/// \return Clock counter value in milliseconds.
	/// \note Timer functionality is based on this function.

	static Clock clock() noexcept;

private:

	Clock _time;
	bool  _started;
	Clock _start_time;
};

/// Rate counter.

class Y_API RateCounter // NOTE: Rename.
{
public:

	RateCounter() noexcept
		: _rate(0)
	{
	}

	/// Start the counter.
	/// \note Should be called just before the beginning of the counted loop.

	void start() noexcept;

	/// Mark a tick.
	/// \note Should be called in the very end of the counted loop.

	void tick() noexcept;

	/// Return the current rate.
	/// \return Current rate.

	size_t rate() const noexcept
	{
		return _rate;
	}

private:

	Clock  _last_time;
	Clock  _total_time;
	size_t _ticks; // NOTE: Why size_t?
	size_t _rate;
};

} // namespace Yttrium

#endif // __Y_TIME_H
