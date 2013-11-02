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

public:

	static void enter(T *pointer, const char *message)
	{
		Y_ABORT_IF(instance, message);
		instance = pointer;
	}

	static void leave(T *pointer)
	{
		Y_ASSERT(instance == pointer);
		instance = nullptr;
	}
};

template <typename T>
T *InstanceGuard<T>::instance = nullptr;

} // namespace Yttrium

#endif // __BASE_INSTANCE_GUARD_H
