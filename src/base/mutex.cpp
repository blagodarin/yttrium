#include "mutex.h"

namespace Yttrium
{

Mutex::Mutex(Allocator *allocator)
	: _private(Y_NEW(allocator, Private)(allocator))
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
	Private::assign(&_private, mutex._private);

	return *this;
}

} // namespace Yttrium
