#ifndef __BASE_ALLOCATABLE_H
#define __BASE_ALLOCATABLE_H

#include <yttrium/allocator.h>

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

	template <typename... Args>
	Allocatable(Args&&... args, Allocator* allocator)
		: _allocator(allocator)
		, _pointer(Y_NEW(_allocator, T)(std::forward<Args>(args)..., _allocator))
	{
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

	T* operator ->()
	{
		return _pointer;
	}

	const T* operator ->() const
	{
		return _pointer;
	}

private:

	Allocator* _allocator;
	T*         _pointer;
};

} // namespace Yttrium

#endif // __BASE_ALLOCATABLE_H
