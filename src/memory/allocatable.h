#ifndef __MEMORY_ALLOCATABLE_H
#define __MEMORY_ALLOCATABLE_H

#include <yttrium/allocator.h>

#include <utility> // std::forward

namespace Yttrium
{

template <typename T>
class Allocatable
{
	Y_NONCOPYABLE(Allocatable);

public:

	Allocatable(Allocator* allocator)
		: _allocator(allocator)
		, _pointer(nullptr)
	{
	}

	Allocatable(Allocatable&& allocatable)
		: _allocator(allocatable._allocator)
		, _pointer(allocatable._pointer)
	{
		allocatable._pointer = nullptr;
	}

	~Allocatable()
	{
		clear();
	}

	Allocator* allocator() const
	{
		return _allocator;
	}

	void clear()
	{
		Y_DELETE(_allocator, _pointer);
		_pointer = nullptr;
	}

	T* get()
	{
		return _pointer;
	}

	const T* get() const
	{
		return _pointer;
	}

	T* release()
	{
		T* result = _pointer;
		_pointer = nullptr;
		return result;
	}

	template <typename... Args>
	void reset(Args&&... args)
	{
		Y_DELETE(_allocator, _pointer);
		_pointer = Y_NEW(_allocator, T)(std::forward<Args>(args)..., _allocator);
	}

	template <typename U, typename... Args>
	void reset(Args&&... args)
	{
		Y_DELETE(_allocator, _pointer);
		_pointer = Y_NEW(_allocator, U)(std::forward<Args>(args)..., _allocator);
	}

	operator bool() const
	{
		return _pointer;
	}

	bool operator!() const
	{
		return !_pointer;
	}

	T* operator->()
	{
		return _pointer;
	}

	const T* operator->() const
	{
		return _pointer;
	}

private:

	Allocator* _allocator;
	T*         _pointer;
};

} // namespace Yttrium

#endif // __MEMORY_ALLOCATABLE_H
