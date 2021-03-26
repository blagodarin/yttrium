// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <filesystem>
#include <memory>

namespace Yt
{
	class Buffer;
	class Source;
	class TemporaryFile;

	///
	class Writer
	{
	public:
		/// Creates a Writer for a Buffer.
		explicit Writer(Buffer&);

		/// Creates a Writer for a file.
		explicit Writer(const std::filesystem::path&);

		/// Creates a Writer for a temporary file.
		explicit Writer(TemporaryFile&);

		/// Returns the current offset.
		uint64_t offset() const noexcept;

		///
		bool try_reserve(uint64_t) noexcept;

		/// Sets the current offset.
		bool seek(uint64_t) noexcept;

		/// Returns the size of the target data.
		uint64_t size() const noexcept;

		/// Writes data to the target.
		size_t write(const void*, size_t) noexcept;

		/// Writes data to the target.
		template <typename T>
		bool write(const T& data) noexcept { return write(&data, sizeof data) == sizeof data; }

		/// Writes data to the target.
		bool write_all(const void* data, size_t size) noexcept { return write(data, size) == size; }

		///
		bool write_all(const Buffer&) noexcept;

		///
		bool write_all(const Source&);

	private:
		std::unique_ptr<class WriterPrivate> _private;
		friend WriterPrivate;

	public:
		Writer() noexcept;
		Writer(Writer&&) noexcept;
		~Writer() noexcept;
		Writer& operator=(Writer&&) noexcept;
		explicit operator bool() const noexcept { return static_cast<bool>(_private); }
	};
}
