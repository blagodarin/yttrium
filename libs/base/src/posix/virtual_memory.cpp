// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "../virtual_memory.h"

#include "error.h"

#include <cstring>

#include <sys/mman.h> // mmap, munmap
#include <unistd.h>   // sysconf

namespace Yt
{
	void* vm_allocate(size_t size) noexcept
	{
		const auto result = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (result != MAP_FAILED)
			return result;
		report_errno("mmap");
		return nullptr;
	}

	void vm_deallocate(void* pointer, size_t size) noexcept
	{
		if (::munmap(pointer, size) != 0)
			report_errno("munmap");
	}

	size_t vm_granularity() noexcept
	{
		if (const auto page_size = ::sysconf(_SC_PAGESIZE); page_size > 0)
			return static_cast<size_t>(page_size);
		report_errno("sysconf(_SC_PAGESIZE)");
		return 1;
	}

	void* vm_reallocate(void* old_pointer, size_t old_size, size_t new_size) noexcept
	{
		// 'mremap' is Linux-specific and known to be very slow.
		const auto new_pointer = ::mmap(nullptr, new_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (new_pointer == MAP_FAILED)
		{
			report_errno("mmap");
			return nullptr;
		}
		std::memcpy(new_pointer, old_pointer, old_size < new_size ? old_size : new_size);
		if (::munmap(old_pointer, old_size) != 0)
			report_errno("munmap");
		return new_pointer;
	}
}
