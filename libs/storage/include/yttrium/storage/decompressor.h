// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace Yt
{
	class Decompressor
	{
	public:
		static std::unique_ptr<Decompressor> zlib();

		virtual ~Decompressor() noexcept = default;
		[[nodiscard]] virtual bool decompress(void* dst, size_t dstCapacity, const void* src, size_t srcSize) noexcept = 0;
	};
}
