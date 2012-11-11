#include "thread.h"

namespace Yttrium
{

Thread::Thread(Allocator *allocator)
	: _allocator(allocator)
	, _private(Y_NEW(_allocator, Private))
{
}

Thread::~Thread()
{
	if (_private->_is_running)
	{
		Y_ABORT("A thread must be terminated explicitly"); // NOTE: Safe to continue (Y_ASSERT?).
		stop();
	}

	Y_DELETE(_allocator, _private);
}

} // namespace Yttrium
