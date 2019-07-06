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

#include <memory>
#include <stdexcept>

namespace Yttrium
{
	class Color4f;
	class Source;

	class Y_CORE_API IonToken
	{
	public:
		enum class Type
		{
			Name,
			StringValue,
			ColorValue,
			ListBegin,
			ListEnd,
			ObjectBegin,
			ObjectEnd,
			End,
		};

		constexpr IonToken(std::size_t line, std::ptrdiff_t column, Type type, std::string_view text, bool translatable = false) noexcept
			: _line{ line }, _column{ column }, _type{ type }, _text{ text }, _translatable{ translatable } {}

		constexpr std::ptrdiff_t column() const noexcept { return _column; }
		constexpr std::size_t line() const noexcept { return _line; }
		constexpr std::string_view text() const noexcept { return _text; }
		constexpr bool translatable() const noexcept { return _translatable; }
		constexpr Type type() const noexcept { return _type; }

		void check_end() const;
		void check_list_begin() const;
		void check_list_end() const;
		void check_name(std::string_view) const;
		void check_object_begin() const;
		void check_object_end() const;
		IonToken& next(class IonReader&);
		Color4f to_color() const;
		std::string_view to_name() const;
		std::string_view to_value() const;

	private:
		std::size_t _line;
		std::ptrdiff_t _column;
		Type _type;
		std::string_view _text;
		bool _translatable;
	};

	class Y_CORE_API IonReader
	{
	public:
		explicit IonReader(const Source&);
		~IonReader();

		IonToken read();

	private:
		const std::unique_ptr<class IonReaderPrivate> _private;
	};
}
