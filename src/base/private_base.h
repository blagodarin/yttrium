#ifndef __BASE_PRIVATE_BASE_H
#define __BASE_PRIVATE_BASE_H

#include <yttrium/allocator.h>
#include <yttrium/assert.h>

#include <atomic>
#include <utility> // std::forward

namespace Yttrium
{

template <typename T>
class PrivateBase
{
	Y_NONCOPYABLE(PrivateBase);

public:

	PrivateBase(Allocator* allocator)
		: _allocator(allocator)
		, _references(allocator ? 1 : 0)
	{
	}

	~PrivateBase()
	{
		Y_ASSERT(_references == 0);
	}

public:

	static T* copy(T* object)
	{
		if (!object)
			return nullptr;
		Y_ASSERT(object->_allocator);
		// NOTE: The check should work well if a thread is trying to copy an object
		// which is being destroyed, but with two or more copying threads only one
		// will end up with nullptr with others having an invalid pointer.
		if (object->_references++ == 0)
			return nullptr;
		return object;
	}

	static void copy(T*& target, T* const source)
	{
		// The source's reference count should be incremented before the target's is decremented;
		// this transparently solves the (rare) problem of assigning an object to itself.
		if (source)
			++source->_references;
		if (target && !--target->_references)
			Y_DELETE(target->_allocator, target);
		target = source;
	}

	static void move(T*& target, T*& source)
	{
		if (target && !--target->_references)
			Y_DELETE(target->_allocator, target);
		target = source;
		source = nullptr;
	}

	static bool release(T* object)
	{
		Allocator *allocator = object->_allocator;

		if (allocator && !--object->_references)
		{
			Y_DELETE(allocator, object);
			return true;
		}

		return false;
	}

	static void release(T** object_ptr)
	{
		T* object = *object_ptr;
		if (object)
		{
			Allocator* allocator = object->_allocator;
			if (allocator) // Otherwise it's an object with static private data.
			{
				if (!--object->_references)
					Y_DELETE(allocator, object);
				*object_ptr = nullptr;
			}
		}
	}

public:

	Allocator* const _allocator;

#ifndef __YTTRIUM_TEST
private:
#else
public:
#endif

	std::atomic_size_t _references;
};

template <typename T>
class PrivateHolder
{
	Y_NONCOPYABLE(PrivateHolder);

public:

	template <typename... Args>
	PrivateHolder(Args&&... args, Allocator* allocator)
		: _pointer(Y_NEW(allocator, T)(std::forward<Args>(args)..., allocator))
	{
	}

	~PrivateHolder()
	{
		PrivateBase<T>::release(&_pointer);
	}

	T* operator->() const
	{
		return _pointer;
	}

	T* release()
	{
		const auto result = _pointer;
		_pointer = nullptr;
		return result;
	}

private:

	T* _pointer;
};

} // namespace Yttrium

#endif // __BASE_PRIVATE_BASE_H
