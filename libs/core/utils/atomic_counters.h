#ifndef _libs_yttrium_utils_atomic_counters_h_
#define _libs_yttrium_utils_atomic_counters_h_

#include <atomic>

namespace Yttrium
{
	template <typename T>
	class MaxCounter
	{
	public:
		T current_value() const { return _value; }
		T maximum_value() const { return _maximum; }

		void operator+=(T difference)
		{
			auto old_value = _value.fetch_add(difference);
			const auto new_value = old_value + difference;
			while (!_maximum.compare_exchange_strong(old_value, new_value))
				if (old_value >= new_value)
					break;
		}

		void operator-=(T difference)
		{
			_value.fetch_sub(difference);
		}

	private:
		std::atomic<T> _value{0};
		std::atomic<T> _maximum{0};
	};

	template <typename T>
	void operator++(MaxCounter<T>& counter)
	{
		counter += 1;
	}

	template <typename T>
	void operator--(MaxCounter<T>& counter)
	{
		counter -= 1;
	}
}

#endif
