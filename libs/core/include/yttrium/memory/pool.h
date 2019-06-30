//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <cstddef>

namespace Yttrium
{
	///
	class Y_CORE_API PoolBase
	{
	public:
		///
		PoolBase(size_t chunk_items, size_t item_size) noexcept;

		///
		~PoolBase();

		PoolBase(const PoolBase&) = delete;
		PoolBase& operator=(const PoolBase&) = delete;

	protected:
		void* allocate();
		void deallocate(void*) noexcept;
		void* take() noexcept;

	private:
		const size_t _chunk_items;
		const size_t _item_size;
		const size_t _chunk_size;
		class PoolChunk* _last_chunk = nullptr;
	};

	///
	template <typename T>
	class Pool : public PoolBase
	{
	public:
		///
		explicit Pool(size_t chunk_items = 32) noexcept
			: PoolBase{ chunk_items, sizeof(T) }
		{
		}

		///
		~Pool()
		{
			clear();
		}

		///
		T* allocate()
		{
			return static_cast<T*>(PoolBase::allocate());
		}

		///
		void clear() noexcept
		{
			for (auto pointer = take(); pointer; pointer = take())
			{
				static_cast<T*>(pointer)->~T();
				PoolBase::deallocate(pointer);
			}
		}

		///
		void deallocate(T* pointer) noexcept
		{
			if (pointer)
			{
				pointer->~T();
				PoolBase::deallocate(pointer);
			}
		}
	};
}
