#include "thread.h"

namespace Yttrium
{

Thread::Thread(Allocator *allocator)
	: _allocator(allocator)
	, _private(new(_allocator->allocate<Private>()) Private())
{
}

Thread::~Thread()
{
	if (_private->_is_running)
	{
		Y_ABORT("A thread must be terminated explicitly"); // NOTE: Safe to continue (Y_ASSERT?).
		stop();
	}

	_allocator->delete_(_private);
}

} // namespace Yttrium
