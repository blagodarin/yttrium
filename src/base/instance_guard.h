#ifndef __BASE_INSTANCE_GUARD_H
#define __BASE_INSTANCE_GUARD_H

#include <yttrium/assert.h>

namespace Yttrium
{

// We don't want to exhibit something like this to the unfriendly outsideness,
// but want to duplicate our code even less. So, here we are.

template <typename T>
class InstanceGuard
{
public:

	static T *instance;

	InstanceGuard(T *pointer, const char *message)
	{
		Y_ABORT_IF(instance, message);
		instance = pointer;
	}

	~InstanceGuard()
	{
		instance = nullptr;
	}
};

template <typename T>
T *InstanceGuard<T>::instance = nullptr;

} // namespace Yttrium

#endif // __BASE_INSTANCE_GUARD_H
