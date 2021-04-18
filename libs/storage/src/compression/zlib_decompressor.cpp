// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/storage/decompressor.h>

#include <cstring>

#include <zlib.h>

namespace
{
	class ZlibDecompressor final : public Yt::Decompressor
	{
	public:
		ZlibDecompressor() noexcept
		{
			std::memset(&_stream, 0, sizeof _stream);
		}

		[[nodiscard]] bool decompress(void* dst, size_t dstCapacity, const void* src, size_t srcSize) noexcept override
		{
			if (inflateInit(&_stream) != Z_OK)
				return false;
			_stream.next_in = static_cast<const Bytef*>(src);
			_stream.avail_in = static_cast<uInt>(srcSize); // TODO: Check.
			_stream.next_out = static_cast<Bytef*>(dst);
			_stream.avail_out = static_cast<uInt>(dstCapacity); // TODO: Check.
			const auto status = ::inflate(&_stream, Z_FINISH);
			::inflateEnd(&_stream);
			return status == Z_STREAM_END;
		}

	private:
		z_stream _stream;
	};
}

namespace Yt
{
	std::unique_ptr<Yt::Decompressor> Yt::Decompressor::zlib()
	{
		return std::make_unique<ZlibDecompressor>();
	}
}
