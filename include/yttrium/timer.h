/// \file
/// \brief Timer.

#ifndef __Y_TIMER_H
#define __Y_TIMER_H

#include <yttrium/global.h>

#include <cstdint>

namespace Yttrium
{
	/// Integer value representing timer time.
	/// \note Passing negative clock values to functions results in undefined behavior.
	typedef int_fast32_t Clock;

	/// %Timer class.
	class Y_API Timer
	{
	public:

		/// Return \c true if the timer is started.
		/// \return \c true for a started timer.
		bool is_started() const { return _is_started; }

		/// Reset the timer time without changing any other state.
		/// \return Timer value on reset.
		Clock reset();

		/// Reset the timer and start it.
		/// \return Timer value on restart.
		Clock restart();

		/// Set the stored time to the specified value.
		/// \param time Time to set.
		void set_time(Clock time) { _time = time; }

		/// Start the timer if it's not started.
		void start();

		/// Stop the timer.
		/// \return Timer value on stop.
		Clock stop();

		/// Get the current timer time.
		/// \return Timer time.
		Clock time() const { return _is_started ? _time + (clock() - _start_time) : _time; }

	public:

		/// Return the current clock counter value.
		/// \return Clock counter value in milliseconds.
		/// \note Timer functionality is based on this function.
		static Clock clock();

	private:

		Clock _time = 0;
		bool  _is_started = false;
		Clock _start_time = 0;
	};
}

#endif // __Y_TIMER_H
