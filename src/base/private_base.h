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

	static void free(T **object_ptr)
	{
		T         *object    = *object_ptr;
		Allocator *allocator = object->_allocator;

		if (allocator)
		{
			allocator->delete_(object);
			*object_ptr = nullptr;
		}
	}

	static bool should_free(T **object_ptr)
	{
		T *object = *object_ptr;

		if (!object)
		{
			return false;
		}

		if (!object->_allocator)
		{
			return true;
		}

		bool result = !--object->_references;

		if (!result)
		{
			*object_ptr = nullptr;
		}

		return result;
	}

protected:

	Allocator *_allocator;

private:

	std::atomic_size_t _references;
};

} // namespace Yttrium

#endif // __BASE_PRIVATE_BASE_H
