#ifndef __BASE_PRIVATE_BASE_H
#define __BASE_PRIVATE_BASE_H

#include <yttrium/allocator.h>
#include <yttrium/assert.h>

#include <atomic>

namespace Yttrium
{

template <typename T>
class PrivateBase
{
	Y_NONCOPYABLE(PrivateBase);

public:

	PrivateBase(Allocator *allocator)
		: _allocator(allocator)
		, _references(allocator ? 1 : 0)
	{
	}

	~PrivateBase()
	{
		Y_ASSERT(_references == 0);
	}

public:

	static void assign(T **target_ptr, T *source)
	{
		if (Y_LIKELY(*target_ptr != source))
		{
			release(target_ptr);
			*target_ptr = copy(source);
		}
	}

	static T *copy(T *object)
	{
		if (object)
		{
			Y_ASSERT(object->_allocator);

			if (object->_references++ > 0)
			{
				return object;
			}
		}

		return nullptr;
	}

	static bool release(T *object)
	{
		Allocator *allocator = object->_allocator;

		if (allocator && !--object->_references)
		{
			Y_DELETE(allocator, object);
			return true;
		}

		return false;
	}

	static void release(T **object_ptr)
	{
		T *object = *object_ptr;
		if (object)
		{
			Allocator *allocator = object->_allocator;
			if (Y_LIKELY(allocator)) // Otherwise it's an object with static private data.
			{
				if (!--object->_references)
					Y_DELETE(allocator, object);
				*object_ptr = nullptr;
			}
		}
	}

public:

	Allocator *_allocator;

#ifndef __YTTRIUM_TEST
private:
#else
public:
#endif

	std::atomic_size_t _references;
};

} // namespace Yttrium

#endif // __BASE_PRIVATE_BASE_H
