/// \file
/// \brief Reference-counted objects.

#ifndef __Y_OBJECT_HPP
#define __Y_OBJECT_HPP

#include <Yttrium/allocators.hpp>
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

	Object(size_t counter = 1) throw()
		: _counter(counter)
	{
	}

	/// The destructor.

	virtual ~Object() throw();

private:

	void increment() throw();

	void decrement() throw();

private:

	size_t _counter;
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

	ObjectPtr() throw()
		: _object(NULL)
	{
	}

	ObjectPtr(const ObjectPtr &object) throw()
		: _object(const_cast<T *>(object._object))
	{
		if (_object)
		{
			_object->increment();
		}
	}

	explicit ObjectPtr(T *object) throw()
		: _object(object)
	{
	}

	~ObjectPtr() throw()
	{
		if (_object)
		{
			_object->decrement();
		}
	}

public:

	/// Explicitly release the object.

	void free() throw()
	{
		if (_object)
		{
			_object->decrement();
			_object = NULL;
		}
	}

	/// Assign an object and lock it.
	/// \param object Object to assign.

	void set(T *object) throw()
	{
		if (_object)
		{
			_object->decrement();
		}
		_object = object;
		if (object)
		{
			object->increment();
		}
	}

	/// Get a raw object pointer.
	/// \return Object pointer.

	T *pointer() throw()
	{
		return _object;
	}

	/**
	* \overload
	*/

	const T *pointer() const throw()
	{
		return _object;
	}

public:

	///

	ObjectPtr &operator =(const ObjectPtr &object) throw()
	{
		if (_object)
		{
			_object->decrement();
		}
		if ((_object = object._object))
		{
			_object->increment();
		}
		return *this;
	}

	///

	T &operator *() throw()
	{
		return *_object;
	}

	///

	const T &operator *() const throw()
	{
		return *_object;
	}

	///

	T *operator ->() throw()
	{
		return _object;
	}

	///

	const T *operator ->() const throw()
	{
		return _object;
	}

	///

	operator SafeBool() const throw()
	{
		return Y_SAFE_BOOL(_object);
	}

	///

	bool operator !() const throw()
	{
		return !_object;
	}

private:

	T *_object;
};

} // namespace Yttrium

#endif // __Y_OBJECT_HPP
