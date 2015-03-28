#include <yttrium/pointer.h>

#include <yttrium/allocator.h>

namespace Yttrium
{

Pointable::Pointer::Pointer(Pointable* pointable)
	: _pointable(pointable)
{
	if (_pointable)
		++_pointable->_counter;
}

void Pointable::Pointer::reset(Pointable* pointable)
{
	if (_pointable && !--_pointable->_counter)
		Y_DELETE(_pointable->_allocator, _pointable);
	_pointable = pointable;
	if (_pointable)
		++_pointable->_counter;
}

Pointable::Pointer& Pointable::Pointer::operator=(Pointer&& pointer)
{
	if (_pointable && !--_pointable->_counter)
		Y_DELETE(_pointable->_allocator, _pointable);
	_pointable = pointer._pointable;
	pointer._pointable = nullptr;
	return *this;
}

} // namespace Yttrium
