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

#include <type_traits>

namespace Yttrium
{
	template <typename T>
	class Flags
	{
	public:
		constexpr Flags() noexcept = default;
		// cppcheck-suppress noExplicitConstructor
		constexpr Flags(const T flag) noexcept
			: _flags{ static_cast<std::underlying_type_t<T>>(flag) } {}
		constexpr explicit operator T() const noexcept { return static_cast<T>(_flags); }
		constexpr explicit operator std::underlying_type_t<T>() const noexcept { return _flags; }
		constexpr Flags& operator|=(const Flags other) noexcept
		{
			_flags |= other._flags;
			return *this;
		}

	private:
		std::underlying_type_t<T> _flags = 0;
	};

	template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	constexpr bool operator==(const Flags<T> a, const Flags<T> b) noexcept
	{
		using U = std::underlying_type_t<T>;
		return static_cast<U>(a) == static_cast<U>(b);
	}

	template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	constexpr bool operator!=(const Flags<T> a, const Flags<T> b) noexcept
	{
		return !(a == b);
	}

	template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	constexpr auto operator&(const Flags<T> a, const T b) noexcept
	{
		using U = std::underlying_type_t<T>;
		return static_cast<U>(a) & static_cast<U>(b);
	}

	template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	constexpr auto operator&(const T a, const Flags<T> b) noexcept
	{
		using U = std::underlying_type_t<T>;
		return static_cast<U>(a) & static_cast<U>(b);
	}

	template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	constexpr auto operator&(const Flags<T> a, const Flags<T> b) noexcept
	{
		using U = std::underlying_type_t<T>;
		return static_cast<U>(a) & static_cast<U>(b);
	}

	template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
	constexpr Flags<T> operator|(const Flags<T> a, const Flags<T> b) noexcept
	{
		using U = std::underlying_type_t<T>;
		return static_cast<T>(static_cast<U>(a) | static_cast<U>(b));
	}

	namespace Operators
	{
		template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
		constexpr Flags<T> operator|(const T a, const T b) noexcept
		{
			using U = std::underlying_type_t<T>;
			return static_cast<T>(static_cast<U>(a) | static_cast<U>(b));
		}
	}
}
