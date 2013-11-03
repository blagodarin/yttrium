#include <yttrium/pointer.h>

#include <yttrium/allocator.h>

namespace Yttrium
{

Pointable::Pointer::Pointer(Pointable* pointable)
	: _pointable(pointable)
{
	if (Y_LIKELY(_pointable))
		++_pointable->_counter;
}

void Pointable::Pointer::reset(Pointable* pointable)
{
	if (_pointable && !--_pointable->_counter)
		Y_DELETE(_pointable->_allocator, _pointable);
	_pointable = pointable;
}

Pointable::Pointer& Pointable::Pointer::operator=(const Pointer& pointer)
{
	reset(pointer._pointable);
	if (_pointable)
		++_pointable->_counter;
	return *this;
}

} // namespace Yttrium
