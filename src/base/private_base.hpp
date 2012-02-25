#ifndef __BASE_PRIVATE_BASE_HPP
#define __BASE_PRIVATE_BASE_HPP

#include <atomic>

#include <Yttrium/allocator.hpp>
#include <Yttrium/assert.hpp>

namespace Yttrium
{

template <typename T>
class PrivateBase
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

	PrivateBase(const PrivateBase &) = delete;

	PrivateBase &operator =(const PrivateBase &) = delete;

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

	static bool should_free(T *object)
	{
		return (object
			? (!object->_allocator || !--object->_references)
			: false);
	}

private:

	Allocator          *_allocator;
	std::atomic_size_t  _references;
};

} // namespace Yttrium

#endif // __BASE_PRIVATE_BASE_HPP
