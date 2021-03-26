// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>

namespace Yt
{
	void* vm_allocate(size_t) noexcept;
	void vm_deallocate(void*, size_t) noexcept;
	size_t vm_granularity() noexcept;
	void* vm_reallocate(void*, size_t, size_t) noexcept;
}
