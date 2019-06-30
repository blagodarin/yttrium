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

#include <yttrium/script/value.h>

#include <yttrium/utils/string.h>

#include <charconv>

namespace Yttrium
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
		append_to(_value, value);
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
		, _value{ make_string(value) }
	{
	}

	ScriptValue::ScriptValue(std::string_view value, Type type)
		: _type{ type }
		, _value{ value }
	{
	}
}
