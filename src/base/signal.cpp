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

Signal::~Signal()
{
	close();
}

Signal &Signal::operator =(const Signal &signal)
{
	close();

	_private = Private::copy(signal._private);

	return *this;
}

} // namespace Yttrium
