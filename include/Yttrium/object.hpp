/// \file
/// \brief Reference-counted objects.

#ifndef __Y_OBJECT_HPP
#define __Y_OBJECT_HPP

#include <Yttrium/allocators.hpp>
#include <Yttrium/assert.hpp>
#include <Yttrium/safe_bool.hpp>
#include <Yttrium/types.hpp>

namespace Yttrium
{

/// Reference-counted object that deletes itself when the counter reaches zero.

class Y_API Object: public Allocatable
{
public:

	/// Increment the reference counter.

	void operator ++() throw()
	{
		++_counter;
	}

	/// Decrement the reference counter, deleting the object if required.

	void operator --() throw()
	{
		Y_ASSERT(_counter > 0);

		if (!--_counter)
		{
			delete this;
		}
	}

protected:

	///

	Object(size_t counter = 1) throw()
		: _counter(counter)
	{
	}

	///

	virtual ~Object() throw()
	{
		Y_ASSERT(_counter == 0);
	}

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
			++*_object;
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
			--*_object;
		}
	}

	ObjectPtr &operator =(const ObjectPtr &object) throw()
	{
		if (_object)
		{
			--*_object;
		}
		if ((_object = object._object))
		{
			++*_object;
		}
		return *this;
	}

	T &operator *() throw()
	{
		return *_object;
	}

	const T &operator *() const throw()
	{
		return *_object;
	}

	T *operator ->() throw()
	{
		return _object;
	}

	const T *operator ->() const throw()
	{
		return _object;
	}

	operator SafeBool() const throw()
	{
		return Y_SAFE_BOOL(_object);
	}

	bool operator !() const throw()
	{
		return !_object;
	}

	/// Explicitly release the object.

	void free() throw()
	{
		if (_object)
		{
			--*_object;
		}
		_object = NULL;
	}

	/// Assign an object and lock it.
	/// \param object Object to assign.

	void set(T *object) throw()
	{
		if (_object)
		{
			--*_object;
		}
		_object = object;
		if (object)
		{
			++*object;
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

private:

	T *_object;
};

} // namespace Yttrium

#endif // __Y_OBJECT_HPP
