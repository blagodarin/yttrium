#include "memory/buffer_memory.h"
#include "memory/buffer_memory_tracker.h"

namespace Yttrium
{
#if Y_ENABLE_BUFFER_MEMORY_TRACKING
	BufferMemoryTracker _buffer_memory_tracker; // Must outlive BufferMemory.
#endif
	BufferMemory _buffer_memory;
}
