// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>

namespace Yt
{
	void convert_normalize_f32_i16(void* dst, const void* src, size_t count) noexcept;

	void convert_normalize_duplicate_f32_i16(void* dst, const void* src, size_t src_count) noexcept;

	void duplicate_i16(void* dst, const void* src, size_t src_count) noexcept;
	void duplicate_f32(void* dst, const void* src, size_t src_count) noexcept;
}
