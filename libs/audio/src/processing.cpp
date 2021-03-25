// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "processing.h"

namespace Yt
{
	void convert_normalize_f32_i16(void* dst, const void* src, size_t count) noexcept
	{
		for (size_t i = 0; i < count; ++i)
			static_cast<float*>(dst)[i] = static_cast<float>(static_cast<const int16_t*>(src)[i]) / 32768.f;
	}

	void convert_normalize_duplicate_f32_i16(void* dst, const void* src, size_t src_count) noexcept
	{
		for (size_t i = 0; i < src_count; ++i)
		{
			const auto x = static_cast<float>(static_cast<const int16_t*>(src)[i]) / 32768.f;
			static_cast<float*>(dst)[2 * i] = x;
			static_cast<float*>(dst)[2 * i + 1] = x;
		}
	}

	void duplicate_i16(void* dst, const void* src, size_t src_count) noexcept
	{
		for (size_t i = 0; i < src_count; ++i)
		{
			const auto x = static_cast<const int16_t*>(src)[i];
			static_cast<int16_t*>(dst)[2 * i] = x;
			static_cast<int16_t*>(dst)[2 * i + 1] = x;
		}
	}

	void duplicate_f32(void* dst, const void* src, size_t src_count) noexcept
	{
		for (size_t i = 0; i < src_count; ++i)
		{
			const auto x = static_cast<const float*>(src)[i];
			static_cast<float*>(dst)[2 * i] = x;
			static_cast<float*>(dst)[2 * i + 1] = x;
		}
	}
}
