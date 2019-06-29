//
// Copyright 2019 Sergei Blagodarin
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
	class TemporaryFile;

	/// Abstract interface to immutable data.
	class Y_CORE_API Source
	{
	public:
		/// Creates a Source from a custom data range. The data must stay valid for the lifetime of the Source.
		static std::unique_ptr<Source> from(const void* data, size_t size);

		/// Creates a Source from a Buffer.
		static std::unique_ptr<Source> from(Buffer&&);

		/// Creates a Source from the specified file.
		static std::unique_ptr<Source> from(const std::string& path);
		static std::unique_ptr<Source> from(std::string_view path) { return from(std::string{ path }); }
		static std::unique_ptr<Source> from(const char* path) { return from(std::string{ path }); }

		/// Creates a Source from a temporary file.
		static std::unique_ptr<Source> from(const TemporaryFile&);

		/// Creates a Source from a part of another Source.
		static std::unique_ptr<Source> from(const std::shared_ptr<const Source>&, uint64_t base, uint64_t size);

		virtual ~Source() noexcept = default;

		/// Returns a pointer to the Source data if it resides in memory, otherwise returns nullptr.
		virtual const void* data() const noexcept { return nullptr; }

		/// Returns the name of the Source.
		std::string_view name() const noexcept { return _name; }

		/// Reads data from the source at the specified offset.
		bool read_all_at(uint64_t offset, void* data, size_t size) const { return read_at(offset, data, size) == size; }

		/// Reads the specified number of bytes of Source data at the specified offset.
		virtual size_t read_at(uint64_t, void*, size_t) const = 0;

		/// Reads data from the source at the specified offset.
		template <typename T>
		bool read_at(uint64_t offset, T& data) const
		{
			return read_at(offset, &data, sizeof data) == sizeof data;
		}

		/// Returns the size of the Source data.
		uint64_t size() const noexcept { return _size; }

		/// Creates a Buffer from the Source data.
		/// The buffer is guaranteed to be null-terminated.
		Buffer to_buffer(std::size_t padding_size = 1) const;

		/// Creates a string from the Source data.
		std::string to_string() const;

	protected:
		const uint64_t _size;
		const std::string _name;

		explicit Source(uint64_t size) noexcept
			: _size{ size } {}
		Source(uint64_t size, const std::string& name)
			: _size{ size }, _name{ name } {}
	};
}
