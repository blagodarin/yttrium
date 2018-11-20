//
// Copyright 2018 Sergei Blagodarin
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
		std::atomic<T> _value{ 0 };
		std::atomic<T> _maximum{ 0 };
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
