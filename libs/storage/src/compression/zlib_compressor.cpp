// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/compressor.h>

#include <cassert>
#include <cstring>

#include <zlib.h>

namespace
{
	class ZlibCompressor final : public Yt::Compressor
	{
	public:
		ZlibCompressor() noexcept
		{
			std::memset(&_stream, 0, sizeof _stream);
		}

		~ZlibCompressor() noexcept override
		{
			::deflateEnd(&_stream);
		}

		[[nodiscard]] bool prepare(int level) noexcept override
		{
			// TODO: Use a previously-prepared compressor without unnecessary deallocations (in deflateEnd)
			// and allocations (in deflateInit). Using deflateParams (and maybe deflateReset) may help.
			assert(level >= 0 && level <= 9);
			::deflateEnd(&_stream);
			return deflateInit(&_stream, level) == Z_OK;
		}

		[[nodiscard]] size_t maxCompressedSize(size_t uncompressedSize) const noexcept override
		{
			// deflateBound DOES return some valid (but suboptimal) bound
			// even for uninitialized streams, so no hard checks here.
			assert(_stream.state);
			return ::deflateBound(const_cast<z_stream*>(&_stream), static_cast<uLong>(uncompressedSize));
		}

		[[nodiscard]] size_t compress(void* dst, size_t dstCapacity, const void* src, size_t srcSize) noexcept override
		{
			_stream.next_in = static_cast<const Bytef*>(src);
			_stream.avail_in = static_cast<uInt>(srcSize); // TODO: Check.
			_stream.next_out = static_cast<Bytef*>(dst);
			_stream.avail_out = static_cast<uInt>(dstCapacity); // TODO: Check.
			const auto status = ::deflate(&_stream, Z_FINISH);
			::deflateEnd(&_stream);
			return status == Z_STREAM_END ? dstCapacity - _stream.avail_out : 0;
		}

	private:
		z_stream _stream;
	};
}

namespace Yt
{
	std::unique_ptr<Yt::Compressor> Yt::Compressor::zlib()
	{
		return std::make_unique<ZlibCompressor>();
	}
}
