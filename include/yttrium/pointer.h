/// \file
/// \brief Reference-counted objects.

#ifndef __Y_POINTER_H
#define __Y_POINTER_H

#include <yttrium/types.h>

#include <atomic>

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

		Pointer() noexcept
			: _pointable(nullptr)
		{
		}

		///

		Pointer(const Pointer& pointer) noexcept
			: Pointer(pointer._pointable)
		{
		}

		///

		Pointer(Pointer&& pointer) noexcept
			: _pointable(pointer._pointable)
		{
			pointer._pointable = nullptr;
		}

		///

		explicit Pointer(Pointable* pointable) noexcept;

		///

		~Pointer() noexcept
		{
			reset();
		}

	public:

		///

		bool is_null() const noexcept
		{
			return !_pointable;
		}

		///

		Pointable* pointer() const noexcept
		{
			return _pointable;
		}

		///

		void reset(Pointable* pointable = nullptr) noexcept;

	public:

		///

		Pointable* operator->() const noexcept
		{
			return _pointable;
		}

		///

		Pointable& operator*() const noexcept
		{
			return *_pointable;
		}

		///

		Pointer& operator=(const Pointer& pointer) noexcept
		{
			reset(pointer._pointable);
			return *this;
		}

		///

		Pointer& operator=(Pointer&& pointer) noexcept;

		///

		bool operator==(const Pointer& pointer) const noexcept
		{
			return _pointable == pointer._pointable;
		}

	private:

		Pointable* _pointable;
	};

	friend Pointer;

public:

	///

	Allocator* allocator() const noexcept
	{
		return _allocator;
	}

protected:

	///

	explicit Pointable(Allocator* allocator) noexcept
		: _allocator(allocator)
		, _counter(0)
	{
	}

	///

	virtual ~Pointable() noexcept
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

	Pointer() noexcept = default;
	Pointer(const Pointer&) noexcept = default;
	Pointer(Pointer&&) noexcept = default;

	Pointer& operator=(const Pointer&) noexcept = default;
	Pointer& operator=(Pointer&&) noexcept = default;

	///

	explicit Pointer(T* pointable) noexcept
		: Pointable::Pointer(pointable)
	{
	}

public:

	///

	T* pointer() const noexcept
	{
		return static_cast<T*>(Pointable::Pointer::pointer());
	}

public:

	///

	T* operator->() const noexcept
	{
		return static_cast<T*>(Pointable::Pointer::pointer());
	}

	///

	T& operator*() const noexcept
	{
		return *static_cast<T*>(Pointable::Pointer::pointer());
	}
};

} // namespace Yttrium

#endif // __Y_POINTER_H
