#include "private.h"

Public::Public(const Public &public_)
	: _private(Private::copy(public_._private))
{
}

void Public::close()
{
	Private::release(&_private);
}

void Public::open(Yttrium::Allocator *allocator)
{
	if (!_private)
	{
		_private = Y_NEW(allocator, Private)(allocator);
	}
}

Public &Public::operator =(const Public &public_)
{
	Private::assign(&_private, public_._private);

	return *this;
}
