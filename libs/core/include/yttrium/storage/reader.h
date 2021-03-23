// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string>

namespace Yt
{
	class Buffer;
	class Source;

	///
	class Reader
	{
	public:
		///
		explicit Reader(const Source& source) noexcept
			: _source{ source } {}

		/// Returns the current offset.
		uint64_t offset() const noexcept { return _offset; }

		/// Reads data from the source.
		size_t read(void*, size_t);

		/// Reads data from the source.
		template <typename T>
		bool read(T& data) { return read(&data, sizeof data) == sizeof data; }

		///
		bool read_line(std::string&);

		/// Sets the current offset.
		bool seek(uint64_t offset) noexcept;

		/// Returns the size of the source data.
		uint64_t size() const noexcept;

		/// Advances the current offset by the specified number of bytes.
		bool skip(uint64_t bytes) noexcept;

	private:
		const Source& _source;
		uint64_t _offset = 0;
	};
}
