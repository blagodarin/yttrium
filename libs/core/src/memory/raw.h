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

#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>

namespace Yttrium
{
	class RawAllocation
	{
	public:
		explicit RawAllocation(size_t size)
			: _pointer{ std::malloc(size) }
		{
			if (!_pointer)
				throw std::bad_alloc{};
		}

		~RawAllocation() noexcept
		{
			std::free(_pointer);
		}

		void* get() const noexcept
		{
			return _pointer;
		}

		void* release() noexcept
		{
			return std::exchange(_pointer, nullptr);
		}

		RawAllocation(const RawAllocation&) = delete;
		RawAllocation& operator=(const RawAllocation&) = delete;

	private:
		void* _pointer = nullptr;
	};

	template <typename T, typename... Args>
	T* new_sized(size_t size, Args&&... args)
	{
		assert(size >= sizeof(T));
		RawAllocation allocation(size);
		new (allocation.get()) T(std::forward<Args>(args)...);
		return static_cast<T*>(allocation.release());
	}

	template <typename T>
	void delete_sized(T* pointer) noexcept
	{
		if (pointer)
		{
			pointer->~T();
			std::free(pointer);
		}
	}
}
