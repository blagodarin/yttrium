// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "utils.h"

namespace Yt
{
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
