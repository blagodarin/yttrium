// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace Yt
{
	// Data compression interface.
	class Compressor
	{
	public:
		static std::unique_ptr<Compressor> zlib();

		virtual ~Compressor() noexcept = default;

		// Prepares for compression. Must be called before every compress() call.
		// Valid level range depends on the algorithm used.
		// This is a separate function (and not a level parameter in other functions)
		// because some compression algorithms (namely zlib) require full initialization
		// before compressed data size estimation.
		[[nodiscard]] virtual bool prepare(int level) noexcept = 0;

		// Returns the maximum compressed data size for uncompressed data of the specified size.
		[[nodiscard]] virtual size_t maxCompressedSize(size_t uncompressedSize) const noexcept = 0;

		// Compresses the input data into the output buffer.
		// Returns the actual compressed data size.
		// The compressor must be prepared again before the next call to compress().
		[[nodiscard]] virtual size_t compress(void* dst, size_t dstCapacity, const void* src, size_t srcSize) noexcept = 0;
	};
}
