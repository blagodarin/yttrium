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
	class SmartPtr
	{
	public:
		constexpr SmartPtr() noexcept = default;

		constexpr SmartPtr(SmartPtr&& other) noexcept
			: _pointer{ other._pointer }
		{
			other._pointer = Sentinel;
		}

		~SmartPtr() noexcept
		{
			if (_pointer != Sentinel)
				Deleter(_pointer);
		}

		constexpr SmartPtr& operator=(SmartPtr&& other) noexcept
		{
			const auto pointer = other._pointer;
			other._pointer = _pointer;
			_pointer = pointer;
		}

		constexpr operator T*() const noexcept
		{
			return _pointer;
		}

		constexpr T* operator->() const noexcept
		{
			return _pointer;
		}

		template <typename U>
		constexpr U* get_as() noexcept
		{
			return reinterpret_cast<U*>(_pointer);
		}

		constexpr T** out() noexcept
		{
			return &_pointer;
		}

		template <typename U>
		constexpr U** out_as() noexcept
		{
			return reinterpret_cast<U**>(&_pointer);
		}

	private:
		T* _pointer = Sentinel;
	};
}
