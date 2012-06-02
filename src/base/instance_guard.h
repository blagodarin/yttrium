#ifndef __BASE_INSTANCE_GUARD_H
#define __BASE_INSTANCE_GUARD_H

#include <Yttrium/assert.h>
#include <Yttrium/static_string.h>

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

	static void enter(T *pointer, const StaticString &message)
	{
		if (instance)
		{
			Y_ABORT(message);
		}

		instance = pointer;
	}

	static void leave(T *pointer, const StaticString &message)
	{
		if (instance != pointer)
		{
			Y_ABORT(message);
		}

		instance = nullptr;
	}
};

template <typename T>
T *InstanceGuard<T>::instance = nullptr;

} // namespace Yttrium

#endif // __BASE_INSTANCE_GUARD_H
