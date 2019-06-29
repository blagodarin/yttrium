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

#include <yttrium/utils/string.h>
#include "string.h"

#include <array>
#include <charconv>
#include <cmath>
#include <limits>

namespace
{
	template <typename T>
	bool float_from_chars(const char* p, const char* end, T& value)
	{
		if (p == end)
			return false;

		bool negative = *p == '-';
		if (negative)
			++p;

		if (p == end)
			return false;

		T result = 0;
		bool has_digits = false;

		if (*p >= '0' && *p <= '9')
		{
			has_digits = true;
			do
			{
				result = result * 10 + static_cast<T>(*p++ - '0');
			} while (p != end && *p >= '0' && *p <= '9');
		}

		if (p != end && *p == '.')
		{
			++p;
			if (p != end && *p >= '0' && *p <= '9')
			{
				has_digits = true;
				T factor = 1;
				do
				{
					result = result * 10 + static_cast<T>(*p++ - '0');
					factor *= 10;
				} while (p != end && *p >= '0' && *p <= '9');
				result /= factor;
			}
		}

		if (!has_digits)
			return false;

		if (p != end && (*p == 'E' || *p == 'e'))
		{
			if (++p == end)
				return false;

			bool negative_power = false;
			switch (*p)
			{
			case '-': negative_power = true; [[fallthrough]];
			case '+': ++p;
			}

			if (p == end || *p < '0' || *p > '9')
				return false;

			T power = 0;
			do
			{
				power = power * 10 + static_cast<T>(*p++ - '0');
			} while (p != end && *p >= '0' && *p <= '9');

			result *= std::pow(T{ 10 }, negative_power ? -power : power);
		}

		if (p != end)
			return false;

		value = negative ? -result : result;
		return true;
	}
}

namespace Yttrium
{
	void _append_to(std::string& string, long long value)
	{
		std::array<char, std::numeric_limits<long long>::digits10 + 2> buffer; // Extra chars for a "lossy" digit and a sign.
		auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
		string.append(buffer.data(), ptr);
	}

	void _append_to(std::string& string, unsigned long long value)
	{
		std::array<char, std::numeric_limits<unsigned long long>::digits10 + 1> buffer; // Extra char for a "lossy" digit.
		auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
		string.append(buffer.data(), ptr);
	}

	void _append_to(std::string& string, Hex32 value)
	{
		std::array<char, 8> buffer;
		auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value._value, 16);
		const auto count = static_cast<std::size_t>(ptr - buffer.data());
		string.append(8 - count, '0');
		string.append(buffer.data(), count);
	}

	void _append_to(std::string& string, double value)
	{
		std::array<char, 32> buffer;
		string.append(buffer.data(), static_cast<size_t>(std::snprintf(buffer.data(), buffer.size(), "%g", value)));
	}

	bool from_chars(std::string_view string, int32_t& value) noexcept
	{
		const auto end = string.data() + string.size();
		auto [ptr, ec] = std::from_chars(string.data(), end, value);
		return ptr == end && ec == std::errc{};
	}

	bool from_chars(std::string_view string, uint32_t& value) noexcept
	{
		const auto end = string.data() + string.size();
		auto [ptr, ec] = std::from_chars(string.data(), end, value);
		return ptr == end && ec == std::errc{};
	}

	bool from_chars(std::string_view string, float& value) noexcept
	{
		return ::float_from_chars(string.data(), string.data() + string.size(), value);
	}

	bool from_chars(std::string_view string, double& value) noexcept
	{
		return ::float_from_chars(string.data(), string.data() + string.size(), value);
	}

	int time_from_chars(std::string_view string) noexcept
	{
		if (string.empty())
			return 0;

		auto p = string.data();
		const auto end = string.data() + string.size();

		bool negative = false;
		switch (*p)
		{
		case '-': negative = true; [[fallthrough]];
		case '+': ++p;
		}

		int result = 0;
		for (; p != end && *p >= '0' && *p <= '9'; ++p)
			result = result * 10 + *p - '0';

		if (p != end && *p == ':')
		{
			int minutes_or_seconds = 0;
			for (++p; p != end && *p >= '0' && *p <= '9'; ++p)
				minutes_or_seconds = minutes_or_seconds * 10 + *p - '0';
			result = result * 60 + minutes_or_seconds;
		}

		if (p != end && *p == ':')
		{
			int seconds = 0;
			for (++p; p != end && *p >= '0' && *p <= '9'; ++p)
				seconds = seconds * 10 + *p - '0';
			result = result * 60 + seconds;
		}

		result *= 1000;

		if (p != end && *p == '.')
		{
			int multiplier = 100;
			for (++p; p != end && *p >= '0' && *p <= '9'; ++p)
			{
				result += (*p - '0') * multiplier;
				multiplier /= 10;
			}
		}

		return negative ? -result : result;
	}
}
