//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "../virtual_memory.h"

#include "error.h"

#include <cstring>

#include <windows.h>

namespace Yttrium
{
	void* vm_allocate(size_t size) noexcept
	{
		const auto result = ::VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!result)
			log_last_error("VirtualAlloc");
		return result;
	}

	void vm_deallocate(void* pointer, size_t) noexcept
	{
		if (!::VirtualFree(pointer, 0, MEM_RELEASE))
			log_last_error("VirtualFree");
	}

	size_t vm_granularity() noexcept
	{
		SYSTEM_INFO system_info;
		::GetSystemInfo(&system_info);
		return system_info.dwPageSize;
	}

	void* vm_reallocate(void* old_pointer, size_t old_size, size_t new_size) noexcept
	{
		const auto new_pointer = ::VirtualAlloc(nullptr, new_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!new_pointer)
			log_last_error("VirtualAlloc");
		else
		{
			std::memcpy(new_pointer, old_pointer, old_size < new_size ? old_size : new_size);
			if (!::VirtualFree(old_pointer, 0, MEM_RELEASE))
				log_last_error("VirtualFree");
		}
		return new_pointer;
	}
}
