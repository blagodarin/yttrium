// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/memory/buffer.h>

namespace Yt
{
	///
	template <typename T>
	class BufferAppender
	{
	public:
		explicit BufferAppender(Buffer& buffer) noexcept
			: _buffer(buffer) {}

		///
		size_t count() const noexcept { return _buffer.size() / sizeof(T); }

		///
		BufferAppender& operator<<(const T& value)
		{
			_buffer.resize(_buffer.size() + sizeof value);
			*reinterpret_cast<T*>(_buffer.end() - sizeof value) = value;
			return *this;
		}

	private:
		Buffer& _buffer;
	};
}
