#include <Yttrium/object.h>

#include <Yttrium/allocator.h>

namespace Yttrium
{

void Object::Pointer::reset(Object *object)
{
	if (_object && !--_object->_counter)
	{
		_object->_allocator->delete_(_object);
	}
	_object = object;
}

Object::Pointer &Object::Pointer::operator =(const Pointer &pointer)
{
	reset(pointer._object);
	if (_object)
	{
		++_object->_counter;
	}
	return *this;
}

} // namespace Yttrium
