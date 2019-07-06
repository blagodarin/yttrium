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

#include <yttrium/script/args.h>

#include <yttrium/script/context.h>
#include <yttrium/script/value.h>

namespace Yttrium
{
	const ScriptValue* ScriptArgs::operator[](size_t index) const
	{
		if (index >= _values.size())
			return nullptr;
		const ScriptValue* value = _values[index];
		return value->type() == ScriptValue::Type::Name ? _context.find(value->string()) : value;
	}
}
