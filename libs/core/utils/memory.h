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

#include <memory>

namespace Yttrium
{
	template <auto Deleter>
	struct UniquePtrDeleter
	{
		template <typename T>
		void operator()(T* p) noexcept
		{
			if (p)
				Deleter(p);
		}
	};

	template <typename T, auto Deleter>
	using UniquePtr = std::unique_ptr<T, UniquePtrDeleter<Deleter>>;

	template <typename T, auto Deleter, T* Sentinel = nullptr>
	class SlimPtr
	{
	public:
		constexpr SlimPtr() noexcept = default;
		constexpr SlimPtr(SlimPtr&& other) noexcept
			: _pointer{ other._pointer } { other._pointer = Sentinel; }
		~SlimPtr() noexcept
		{
			if (_pointer != Sentinel)
				Deleter(_pointer);
		}
		constexpr SlimPtr& operator=(SlimPtr&& other) noexcept
		{
			const auto pointer = other._pointer;
			other._pointer = _pointer;
			_pointer = pointer;
		}
		constexpr operator T*() const noexcept { return _pointer; }
		constexpr T* operator->() const noexcept { return _pointer; }
		constexpr T** operator&() noexcept { return &_pointer; }

	private:
		T* _pointer = Sentinel;
	};
}
