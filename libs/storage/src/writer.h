// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <cstddef>

namespace Yt
{
	class Buffer;
	class Writer;

	class WriterPrivate
	{
	public:
		WriterPrivate() noexcept = default;
		virtual ~WriterPrivate() noexcept = default;

		virtual bool try_reserve(uint64_t) noexcept { return true; }
		virtual size_t write_at(uint64_t, const void*, size_t) noexcept = 0;

	private:
		uint64_t _size = 0;
		uint64_t _offset = 0;

		friend Writer;
	};

	class BufferWriter final : public WriterPrivate
	{
	public:
		explicit BufferWriter(Buffer&) noexcept;

		bool try_reserve(uint64_t) noexcept override;
		size_t write_at(uint64_t, const void*, size_t) noexcept override;

	private:
		Buffer& _buffer;
	};
}
