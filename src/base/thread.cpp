#include "thread.hpp"

namespace Yttrium
{

Thread::Thread(Allocator *allocator)
	: _private(new(allocator->allocate<Private>())
		Private(allocator))
{
}

Thread::Thread(const Thread &thread)
	: _private(Private::copy(thread._private))
{
}

Thread::~Thread()
{
	close();
}

Thread &Thread::operator =(const Thread &thread)
{
	close();

	_private = Private::copy(thread._private);

	return *this;
}

} // namespace Yttrium
