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

#include <memory>
#include <string_view>

namespace Yttrium
{
	class Buffer;
	class Reader;
	class Source;
	class TemporaryFile;

	///
	class Y_CORE_API Writer
	{
	public:
		/// Creates a Writer for a Buffer.
		explicit Writer(Buffer&);

		/// Creates a Writer for the specified file.
		explicit Writer(const std::string& path);

		/// Creates a Writer for a temporary file.
		explicit Writer(TemporaryFile&);

		/// Returns the current offset.
		uint64_t offset() const;

		///
		void reserve(uint64_t);

		///
		void resize(uint64_t);

		/// Sets the current offset.
		bool seek(uint64_t);

		/// Returns the size of the target data.
		uint64_t size() const;

		///
		void unlink();

		/// Writes data to the target.
		size_t write(const void*, size_t);

		/// Writes data to the target.
		template <typename T>
		bool write(const T& data)
		{
			return write(&data, sizeof data) == sizeof data;
		}

		/// Writes data to the target.
		bool write_all(const void* data, size_t size) { return write(data, size) == size; }

		///
		bool write_all(const Buffer&);

		///
		bool write_all(const Source&);

		///
		bool write_all(std::string_view);

		/// Writes data at the specified offset.
		size_t write_at(uint64_t offset, const void* data, size_t size);

		/// Writes data at the specified offset.
		template <typename T>
		bool write_at(uint64_t offset, const T& data)
		{
			return write_at(offset, &data, sizeof data) == sizeof data;
		}

	private:
		std::unique_ptr<class WriterPrivate> _private;
		friend WriterPrivate;

	public:
		Writer() noexcept;
		Writer(Writer&&) noexcept;
		~Writer();
		Writer& operator=(Writer&&) noexcept;
		explicit operator bool() const { return static_cast<bool>(_private); }
	};
}
