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

#include <yttrium/api.h>

#include <string>

namespace Yttrium
{
	class ScriptArgs;
	class ScriptCodePrivate;
	class ScriptContext;
	class ScriptParser;

	/// Script value data.
	class Y_CORE_API ScriptValue
	{
		friend ScriptArgs;
		friend ScriptCodePrivate;
		friend ScriptContext;
		friend ScriptParser;

	public:
		/// Value type.
		/// \note Every Name is a Literal, and every Literal is a String, but not visa versa.
		enum class Type
		{
			String,  ///< String value.
			Literal, ///< Literal value (e.g. integer).
			Name,    ///< Name (e.g. identifier).
		};

	public:
		///
		int to_int() const noexcept;

		///
		Type type() const noexcept { return _type; }

		///
		const std::string& string() const noexcept { return _value; }

	public:
		///
		ScriptValue& operator=(int);

		///
		ScriptValue& operator=(std::string_view);

	private:
		Type _type;
		std::string _value;

	private:
		Y_PRIVATE explicit ScriptValue(int);
		Y_PRIVATE explicit ScriptValue(double);
		Y_PRIVATE explicit ScriptValue(std::string_view, Type = Type::String);
	};
}
