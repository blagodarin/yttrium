// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/script/value.h>

#include <charconv>

#include <fmt/format.h>

namespace Yt
{
	int ScriptValue::to_int() const noexcept
	{
		int result = 0;
		std::from_chars(_value.data(), _value.data() + _value.size(), result);
		return result;
	}

	ScriptValue& ScriptValue::operator=(int value)
	{
		_type = Type::Literal;
		_value.clear();
		fmt::format_to(std::back_inserter(_value), "{}", value);
		return *this;
	}

	ScriptValue& ScriptValue::operator=(std::string_view value)
	{
		_type = Type::String;
		_value = value;
		return *this;
	}

	ScriptValue::ScriptValue(int value)
		: _type{ Type::Literal }
		, _value{ std::to_string(value) }
	{
	}

	ScriptValue::ScriptValue(std::string_view value, Type type)
		: _type{ type }
		, _value{ value }
	{
	}
}
