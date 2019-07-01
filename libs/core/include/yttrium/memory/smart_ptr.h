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

#include <type_traits>

namespace Yttrium
{
	template <typename T, typename Deleter>
	class SmartPtrBase
	{
	public:
		using Value = std::remove_all_extents_t<std::remove_reference_t<T>>;

		constexpr SmartPtrBase() noexcept = default;

		SmartPtrBase(const SmartPtrBase&) = delete;

		constexpr SmartPtrBase(SmartPtrBase&& other) noexcept
			: _pointer{ other._pointer } { other._pointer = Sentinel; }

		constexpr explicit SmartPtrBase(Value* pointer) noexcept
			: _pointer{ pointer } {}

		~SmartPtrBase() noexcept { Deleter::free(_pointer); }

		SmartPtrBase& operator=(const SmartPtrBase&) = delete;

		constexpr SmartPtrBase& operator=(SmartPtrBase&& other) noexcept
		{
			const auto pointer = other._pointer;
			other._pointer = _pointer;
			_pointer = pointer;
			return *this;
		}

		constexpr operator Value*() const noexcept { return _pointer; }

		constexpr Value* operator->() const noexcept { return _pointer; }

		template <typename U>
		constexpr std::enable_if_t<std::is_integral_v<U>, Value>& operator[](U index) const noexcept
		{
			return _pointer[index];
		}

		constexpr Value* get() const noexcept { return _pointer; }

		template <typename U>
		constexpr U* get_as() const noexcept
		{
			return reinterpret_cast<U*>(_pointer);
		}

		constexpr Value** out() noexcept { return &_pointer; }

		template <typename U>
		constexpr std::enable_if_t<std::is_pointer_v<U>, U> out_as() noexcept
		{
			return reinterpret_cast<U>(&_pointer);
		}

		void reset(Value* pointer = Sentinel) noexcept
		{
			Deleter::free(_pointer);
			_pointer = pointer;
		}

	private:
		static constexpr Value* Sentinel = Deleter::template Sentinel<Value>;
		Value* _pointer = Sentinel;
	};

	template <auto deleter>
	struct SmartPtrFunctionDeleter
	{
		template <typename T>
		static constexpr T* Sentinel = nullptr;

		template <typename T>
		static void free(T* pointer) noexcept
		{
			if (pointer)
				deleter(pointer);
		}
	};

	template <typename T, auto deleter>
	using SmartPtr = SmartPtrBase<T, SmartPtrFunctionDeleter<deleter>>;
}
