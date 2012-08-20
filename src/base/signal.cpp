#include "signal.h"

namespace Yttrium
{

Signal::Signal(Allocator *allocator)
	: _private(new(allocator->allocate<Private>())
		Private(allocator))
{
}

Signal::Signal(const Signal &signal)
	: _private(Private::copy(signal._private))
{
}

void Signal::close()
{
	Private::release(&_private);
}

Signal &Signal::operator =(const Signal &signal)
{
	Private::copy(&_private, signal._private);

	return *this;
}

} // namespace Yttrium
