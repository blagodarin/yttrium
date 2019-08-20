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

#include <cstdint>
#include <string>
#include <type_traits>

namespace Yt
{
	struct Hex32
	{
		const std::uint32_t _value;
		template <typename T, typename = std::enable_if_t<std::is_integral_v<T> && sizeof(T) <= sizeof(std::uint32_t)>>
		explicit Hex32(T value) noexcept
			: _value{ static_cast<std::uint32_t>(value) }
		{}
	};

	Y_CORE_API void _append_to(std::string&, long long);
	Y_CORE_API void _append_to(std::string&, unsigned long long);
	Y_CORE_API void _append_to(std::string&, Hex32);
	Y_CORE_API void _append_to(std::string&, double);

	inline void _append_to(std::string& string, char value) { string.append(1, value); }
	inline void _append_to(std::string& string, int value) { _append_to(string, static_cast<long long>(value)); }
	inline void _append_to(std::string& string, long value) { _append_to(string, static_cast<long long>(value)); }
	inline void _append_to(std::string& string, unsigned value) { _append_to(string, static_cast<unsigned long long>(value)); }
	inline void _append_to(std::string& string, unsigned long value) { _append_to(string, static_cast<unsigned long long>(value)); }
	inline void _append_to(std::string& string, float value) { _append_to(string, static_cast<double>(value)); }
	inline void _append_to(std::string& string, std::string_view value) { string.append(value); }

	inline void append_to(std::string&) {}

	///
	template <typename T, typename... Args>
	void append_to(std::string& string, T&& value, Args&&... args)
	{
		_append_to(string, std::forward<T>(value));
		append_to(string, std::forward<Args>(args)...);
	}

	///
	Y_CORE_API bool from_chars(std::string_view, int32_t&) noexcept;
	Y_CORE_API bool from_chars(std::string_view, uint32_t&) noexcept;
	Y_CORE_API bool from_chars(std::string_view, float&) noexcept;
	Y_CORE_API bool from_chars(std::string_view, double&) noexcept;

	///
	template <typename... Args>
	std::string make_string(Args&&... args)
	{
		std::string string;
		append_to(string, std::forward<Args>(args)...);
		return string;
	}
}
