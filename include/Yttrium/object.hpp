/// \file
/// \brief Reference-counted objects.

#ifndef __Y_OBJECT_HPP
#define __Y_OBJECT_HPP

#include <atomic> // atomic_*

#include <Yttrium/allocator.hpp>
#include <Yttrium/safe_bool.hpp>
#include <Yttrium/types.hpp>

namespace Yttrium
{

/// Reference-counted object that deletes itself when the counter reaches zero.

// NOTE: This only adds the reference counter, so maybe it should be renamed to ReferenceCounted or Managed or something.

class Y_API Object: public Allocatable
{
	template <typename T>
	friend class ObjectPtr;

protected:

	/// Initialize the reference counter.
	/// \param counter The initial counter value.
	/// \note Non-ObjectPtr-managed (i.e. non-dynamic) Object descendants
	/// should initialize the \a counter with 0 to avoid the assertion failure upon destruction.

	Object(size_t counter = 1) noexcept
		: _counter(counter)
	{
	}

	/// The destructor.

	virtual ~Object() noexcept;

private:

	void increment() noexcept;

	void decrement() noexcept;

private:

	std::atomic_size_t _counter;
};

/// Object pointer wrapper.
/// \tparam T Type to wrap the pointer to.
/// \note It MUST be used instead of plain Object pointers for variables
/// unless one is ABSOLUTELY sure what's they doing.
/// \note The current implementation is NOT thread-safe!

template <typename T>
class ObjectPtr
{
public:

	ObjectPtr() noexcept
		: _object(nullptr)
	{
	}

	ObjectPtr(const ObjectPtr &object) noexcept
		: _object(const_cast<T *>(object._object))
	{
		if (_object)
		{
			_object->increment();
		}
	}

	explicit ObjectPtr(T *object) noexcept
		: _object(object)
	{
	}

	~ObjectPtr() noexcept
	{
		if (_object)
		{
			_object->decrement();
		}
	}

public:

	/// Explicitly release the object.

	void free() noexcept
	{
		if (_object)
		{
			_object->decrement();
			_object = nullptr;
		}
	}

	/// Assign an object and lock it.
	/// \param object Object to assign.

	void set(T *object) noexcept
	{
		if (_object)
		{
			_object->decrement();
		}
		_object = object;
		if (_object)
		{
			_object->increment();
		}
	}

	/// Get a raw object pointer.
	/// \return Object pointer.

	T *pointer() noexcept
	{
		return _object;
	}

	/**
	* \overload
	*/

	const T *pointer() const noexcept
	{
		return _object;
	}

public:

	///

	ObjectPtr &operator =(const ObjectPtr &object) noexcept
	{
		if (_object)
		{
			_object->decrement();
		}
		_object = object._object;
		if (_object)
		{
			_object->increment();
		}
		return *this;
	}

	///

	T &operator *() noexcept
	{
		return *_object;
	}

	///

	const T &operator *() const noexcept
	{
		return *_object;
	}

	///

	T *operator ->() noexcept
	{
		return _object;
	}

	///

	const T *operator ->() const noexcept
	{
		return _object;
	}

	///

	operator SafeBool() const noexcept
	{
		return Y_SAFE_BOOL(_object);
	}

	///

	bool operator !() const noexcept
	{
		return !_object;
	}

private:

	T *_object;
};

} // namespace Yttrium

#endif // __Y_OBJECT_HPP
