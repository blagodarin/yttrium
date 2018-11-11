#include "../memory.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>

#include <windows.h>

namespace Yttrium
{
	void* pages_allocate(size_t size) noexcept
	{
		const auto result = ::VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (result || ::GetLastError() == ERROR_NOT_ENOUGH_MEMORY)
			return result;
		std::abort();
	}

	void pages_deallocate(void* pointer, size_t) noexcept
	{
		if (!::VirtualFree(pointer, 0, MEM_RELEASE))
			std::abort();
	}

	size_t pages_granularity() noexcept
	{
		SYSTEM_INFO system_info;
		::GetSystemInfo(&system_info);
		return system_info.dwPageSize;
	}

	void* pages_reallocate(void* old_pointer, size_t old_size, size_t new_size) noexcept
	{
		const auto new_pointer = ::VirtualAlloc(nullptr, new_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (new_pointer)
		{
			std::memcpy(new_pointer, old_pointer, std::min(old_size, new_size));
			if (!::VirtualFree(old_pointer, 0, MEM_RELEASE))
				std::abort();
			return new_pointer;
		}
		else if (::GetLastError() == ERROR_NOT_ENOUGH_MEMORY)
			return nullptr;
		std::abort();
	}
}
