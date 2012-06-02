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

Mutex::~Mutex()
{
	close();
}

Mutex &Mutex::operator =(const Mutex &mutex)
{
	close();

	_private = Private::copy(mutex._private);

	return *this;
}

} // namespace Yttrium
