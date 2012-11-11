#include "signal.h"

namespace Yttrium
{

Signal::Signal(Allocator *allocator)
	: _private(Y_NEW(allocator, Private)(allocator))
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
	Private::assign(&_private, signal._private);

	return *this;
}

} // namespace Yttrium
