//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <yttrium/api.h>

#include <filesystem>
#include <memory>

namespace Yt
{
	class Buffer;
	class Source;
	class TemporaryFile;

	///
	class Y_CORE_API Writer
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
