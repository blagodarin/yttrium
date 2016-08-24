#ifndef _src_memory_new_h_
#define _src_memory_new_h_

#include <new>

#define Y_NEW(allocator, Class) new(static_cast<Yttrium::Allocator*>(allocator)->allocate<Class>()) Class

namespace Yttrium
{
	template <typename T>
	void Y_DELETE(Allocator* allocator, T* pointer)
	{
		if (pointer)
		{
			pointer->~T();
			allocator->deallocate(pointer);
		}
	}
}

#endif
