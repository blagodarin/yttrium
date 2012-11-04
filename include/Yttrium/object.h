/// \file
/// \brief Reference-counted objects.

#ifndef __Y_OBJECT_H
#define __Y_OBJECT_H

#include <Yttrium/noncopyable.h>
#include <Yttrium/types.h>

#include <atomic>

namespace Yttrium
{

/// An object with a reference counter.

class Y_API Object
	: public Noncopyable
{
	friend class Allocator;

public:

	///

	class Pointer
	{
	public:

		///

		inline Pointer() noexcept;

		///

		inline Pointer(const Pointer &pointer) noexcept;

		///

		inline Pointer(Pointer &&pointer) noexcept;

		///

		inline explicit Pointer(Object *object) noexcept;

		///

		inline ~Pointer() noexcept;

	public:

		///

		inline bool is_null() const noexcept;

		///

		inline Object *pointer() const noexcept;

		///

		void reset(Object *object = nullptr) noexcept;

	public:

		///

		inline Object *operator ->() const noexcept;

		///

		inline Object &operator *() const noexcept;

		///

		Pointer &operator =(const Pointer &pointer) noexcept;

	private:

		Object *_object;
	};

	friend class Pointer;

public:

	///

	inline Allocator *allocator() const noexcept;

protected:

	///

	inline Object(Allocator *allocator) noexcept;

	///

	inline virtual ~Object() noexcept;

private:

	Allocator           *_allocator;
	std::atomic<size_t>  _counter;
};

///

template <typename T>
class ObjectPointer
	: public Object::Pointer
{
public:

	///

	ObjectPointer() = default;

	///

	explicit ObjectPointer(T *object) noexcept;

public:

	///

	T *pointer() const noexcept;

public:

	///

	T *operator ->() const noexcept;

	///

	T &operator *() const noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Object::Pointer::Pointer() noexcept
	: _object(nullptr)
{
}

Object::Pointer::Pointer(const Pointer &pointer) noexcept
	: _object(pointer._object)
{
	if (_object)
	{
		++_object->_counter;
	}
}

Object::Pointer::Pointer(Pointer &&pointer) noexcept
	: _object(pointer._object)
{
	pointer._object = nullptr;
}

Object::Pointer::Pointer(Object *object) noexcept
	: _object(object)
{
}

Object::Pointer::~Pointer() noexcept
{
	reset();
}

bool Object::Pointer::is_null() const noexcept
{
	return !_object;
}

Object *Object::Pointer::pointer() const noexcept
{
	return _object;
}

Object *Object::Pointer::operator ->() const noexcept
{
	return _object;
}

Object &Object::Pointer::operator *() const noexcept
{
	return *_object;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Allocator *Object::allocator() const noexcept
{
	return _allocator;
}

Object::Object(Allocator *allocator) noexcept
	: _allocator(allocator)
	, _counter(1)
{
}

Object::~Object() noexcept
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
ObjectPointer<T>::ObjectPointer(T *object) noexcept
	: Object::Pointer(object)
{
}

template <typename T>
T *ObjectPointer<T>::pointer() const noexcept
{
	return static_cast<T *>(Object::Pointer::pointer());
}

template <typename T>
T *ObjectPointer<T>::operator ->() const noexcept
{
	return static_cast<T *>(Object::Pointer::pointer());
}

template <typename T>
T &ObjectPointer<T>::operator *() const noexcept
{
	return *static_cast<T *>(Object::Pointer::pointer());
}

} // namespace Yttrium

#endif // __Y_OBJECT_H
