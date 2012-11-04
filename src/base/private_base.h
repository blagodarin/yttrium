#ifndef __BASE_PRIVATE_BASE_H
#define __BASE_PRIVATE_BASE_H

#include <Yttrium/allocator.h>
#include <Yttrium/assert.h>
#include <Yttrium/noncopyable.h>

#include <atomic>

namespace Yttrium
{

template <typename T>
class PrivateBase: public Noncopyable
{
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

	static void release(T *object)
	{
		Allocator *allocator = object->_allocator;

		if (allocator && !--object->_references)
		{
			allocator->delete_(object);
		}
	}

	static void release(T **object_ptr)
	{
		T *object = *object_ptr;

		if (object)
		{
			*object_ptr = nullptr;
			release(object);
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
