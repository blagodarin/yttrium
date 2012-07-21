#include "mutex.h"

namespace Yttrium
{

Mutex::Mutex(Allocator *allocator)
	: _private(new(allocator->allocate<Private>())
		Private(allocator))
{
}

Mutex::Mutex(const Mutex &mutex)
	: _private(Private::copy(mutex._private))
{
}

void Mutex::close()
{
	Private::release(&_private);
}

Mutex &Mutex::operator =(const Mutex &mutex)
{
	close();

	_private = Private::copy(mutex._private);

	return *this;
}

} // namespace Yttrium
