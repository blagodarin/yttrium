#include "memory/buffer_memory.h"

#if Y_ENABLE_BUFFER_MEMORY_TRACKING
	#include "memory/buffer_memory_tracker.h"
#endif

namespace Yttrium
{
#if Y_ENABLE_BUFFER_MEMORY_TRACKING
	BufferMemoryTracker _buffer_memory_tracker; // Must outlive BufferMemory.
#endif
	BufferMemory _buffer_memory;
}
