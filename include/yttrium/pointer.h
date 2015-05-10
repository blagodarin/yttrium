/// \file
/// \brief Reference-counted objects.

#ifndef __Y_POINTER_H
#define __Y_POINTER_H

#include <yttrium/types.h>

#include <atomic>
#include <cstddef>

namespace Yttrium
{

/// An object with a reference counter.

class Y_API Pointable
{
	friend Allocator; // To make Pointable::~Pointable available.

	Y_NONCOPYABLE(Pointable);

public:

	///

	class Pointer
	{
	public:

		///

		Pointer()
			: _pointable(nullptr)
		{
		}

		///

		Pointer(const Pointer& pointer)
			: Pointer(pointer._pointable)
		{
		}

		///

		Pointer(Pointer&& pointer)
			: _pointable(pointer._pointable)
		{
			pointer._pointable = nullptr;
		}

		///

		explicit Pointer(Pointable* pointable);

		///

		~Pointer()
		{
			reset();
		}

	public:

		///

		explicit operator bool() const
		{
			return _pointable;
		}

		///

		Pointable* get() const
		{
			return _pointable;
		}

		///

		void reset(Pointable* pointable = nullptr);

	public:

		///

		Pointable* operator->() const
		{
			return _pointable;
		}

		///

		Pointable& operator*() const
		{
			return *_pointable;
		}

		///

		Pointer& operator=(const Pointer& pointer)
		{
			reset(pointer._pointable);
			return *this;
		}

		///

		Pointer& operator=(Pointer&& pointer);

		///

		bool operator==(const Pointer& pointer) const
		{
			return _pointable == pointer._pointable;
		}

		///

		bool operator!=(const Pointer& pointer) const
		{
			return _pointable != pointer._pointable;
		}

	private:

		Pointable* _pointable;
	};

	friend Pointer;

public:

	///

	Allocator* allocator() const
	{
		return _allocator;
	}

protected:

	///

	explicit Pointable(Allocator* allocator)
		: _allocator(allocator)
		, _counter(0)
	{
	}

	///

	virtual ~Pointable()
	{
	}

private:

	Allocator*          _allocator;
	std::atomic<size_t> _counter;
};

/// Smart pointer to a Pointable descendant.

template <typename T>
class Pointer: public Pointable::Pointer
{
public:

	Pointer() = default;
	Pointer(const Pointer&) = default;
	Pointer(Pointer&&) = default;

	Pointer& operator=(const Pointer&) = default;
	Pointer& operator=(Pointer&&) = default;

	///

	explicit Pointer(T* pointable)
		: Pointable::Pointer(pointable)
	{
	}

public:

	///

	T* get() const
	{
		return static_cast<T*>(Pointable::Pointer::get());
	}

public:

	///

	T* operator->() const
	{
		return static_cast<T*>(Pointable::Pointer::get());
	}

	///

	T& operator*() const
	{
		return *static_cast<T*>(Pointable::Pointer::get());
	}
};

} // namespace Yttrium

#endif // __Y_POINTER_H
