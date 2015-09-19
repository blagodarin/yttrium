#ifndef _src_base_instance_guard_h_
#define _src_base_instance_guard_h_

#include <mutex>

namespace Yttrium
{
	// We don't want to exhibit something like this to the unfriendly outsideness,
	// but want to duplicate our code even less. So, here we are.

	// Members of this class should be the last in the member list to ensure that
	// the instance pointer will be reset before the instance itself is destroyed.

	template <typename T>
	class InstanceGuard
	{
	public:

		static std::mutex instance_mutex;
		static T* instance;

		InstanceGuard(T* pointer, const char* message)
		{
			std::lock_guard<std::mutex> lock(instance_mutex);
			if (instance)
				throw std::logic_error(message);
			instance = pointer;
		}

		~InstanceGuard()
		{
			std::lock_guard<std::mutex> lock(instance_mutex);
			instance = nullptr;
		}
	};

	template <typename T>
	std::mutex InstanceGuard<T>::instance_mutex;

	template <typename T>
	T* InstanceGuard<T>::instance = nullptr;
}

#endif
