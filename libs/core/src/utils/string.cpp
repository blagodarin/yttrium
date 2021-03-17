// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/utils/string.h>

#include <charconv>
#include <cmath>

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

namespace Yt
{
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
}
