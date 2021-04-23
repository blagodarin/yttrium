// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <stdexcept>

namespace Yt
{
	class Source;

	class IonToken
	{
	public:
		enum class Type
		{
			Name,
			StringValue,
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
		[[nodiscard]] bool is_name(std::string_view name) const noexcept { return _type == Type::Name && _text == name; }
		IonToken& next(class IonReader&);
		std::string_view to_name() const;
		std::string_view to_value() const;

	private:
		std::size_t _line;
		std::ptrdiff_t _column;
		Type _type;
		std::string_view _text;
		bool _translatable;
	};

	class IonReader
	{
	public:
		explicit IonReader(const Source&);
		~IonReader();

		IonToken read();

	private:
		const std::unique_ptr<class IonReaderPrivate> _private;
	};
}
