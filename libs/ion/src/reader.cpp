// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/ion/reader.h>

#include <yttrium/exceptions.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/storage/source.h>

#include <array>
#include <vector>

#include <fmt/format.h>

namespace
{
	enum CharClass
	{
		Other,    // Any character not mentioned below.
		End,      // '\0'.
		Space,    // '\t', '\v', '\f' or ' '.
		Cr,       // '\r'.
		Lf,       // '\n'.
		Name,     // 'A'-'Z', 'a'-'z', '0'-'9' or '_'.
		Quote,    // '"', '`'.
		LBrace,   // '{'.
		RBrace,   // '}'.
		LBracket, // '['.
		RBracket, // ']'.
		Comment,  // '/'.
	};

	const std::array<CharClass, 256> char_class{
		End, Other, Other, Other, Other, Other, Other, Other,     // \0
		Other, Space, Lf, Space, Space, Cr, Other, Other,         // \t \n \v \f \r
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Space, Other, Quote, Other, Other, Other, Other, Other,   //   ! " # $ % & '
		Other, Other, Other, Other, Other, Other, Other, Comment, // ( ) * + , - . /
		Name, Name, Name, Name, Name, Name, Name, Name,           // 0 1 2 3 4 5 6 7
		Name, Name, Other, Other, Other, Other, Other, Other,     // 8 9 : ; < = > ?
		Other, Name, Name, Name, Name, Name, Name, Name,          // @ A B C D E F G
		Name, Name, Name, Name, Name, Name, Name, Name,           // H I J K L M N O
		Name, Name, Name, Name, Name, Name, Name, Name,           // P Q R S T U V W
		Name, Name, Name, LBracket, Other, RBracket, Other, Name, // X Y Z [ \ ] ^ _
		Quote, Name, Name, Name, Name, Name, Name, Name,          // ` a b c d e f g
		Name, Name, Name, Name, Name, Name, Name, Name,           // h i j k l m n o
		Name, Name, Name, Name, Name, Name, Name, Name,           // p q r s t u v w
		Name, Name, Name, LBrace, Other, RBrace, Other, Other,    // x y z { | } ~
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
		Other, Other, Other, Other, Other, Other, Other, Other,   //
	};

	CharClass class_of(char c)
	{
		return ::char_class[static_cast<unsigned char>(c)];
	}

	template <typename I, typename P>
	constexpr I forward_find_if(I iterator, P&& predicate)
	{
		for (; !predicate(*iterator); ++iterator)
			;
		return iterator;
	}
}

namespace Yt
{
	void IonToken::check_end() const
	{
		if (_type != Type::End)
			throw IonError{ fmt::format("({}:{}) End of file expected", _line, _column) };
	}

	void IonToken::check_list_begin() const
	{
		if (_type != Type::ListBegin)
			throw IonError{ fmt::format("({}:{}) '[' expected", _line, _column) };
	}

	void IonToken::check_list_end() const
	{
		if (_type != Type::ListEnd)
			throw IonError{ fmt::format("({}:{}) ']' expected", _line, _column) };
	}

	void IonToken::check_name(std::string_view name) const
	{
		if (to_name() != name)
			throw IonError{ fmt::format("({}:{}) '{}' expected", _line, _column, name) };
	}

	void IonToken::check_object_begin() const
	{
		if (_type != Type::ObjectBegin)
			throw IonError{ fmt::format("({}:{}) '{{' expected", _line, _column) };
	}

	void IonToken::check_object_end() const
	{
		if (_type != Type::ObjectEnd)
			throw IonError{ fmt::format("({}:{}) '}}' expected", _line, _column) };
	}

	IonToken& IonToken::next(IonReader& ion)
	{
		return *this = ion.read();
	}

	std::string_view IonToken::to_name() const
	{
		if (_type != Type::Name)
			throw IonError{ fmt::format("({}:{}) ION name expected", _line, _column) };
		return _text;
	}

	std::string_view IonToken::to_value() const
	{
		if (_type != Type::StringValue)
			throw IonError{ fmt::format("({}:{}) ION value expected", _line, _column) };
		return _text;
	}

	class IonReaderPrivate
	{
	public:
		explicit IonReaderPrivate(const Source& source)
			: _buffer{ source.to_buffer() }
		{
		}

		IonToken read()
		{
			for (;;)
			{
				switch (::class_of(*_cursor))
				{
				case Other:
					throw IonError{ fmt::format("({}:{}) Bad character", _line, _cursor - _line_base) };

				case End:
					if (_cursor == static_cast<const char*>(_buffer.data()) + _buffer.size())
						return make_token<IonToken::Type::End>(_cursor, 0);
					else
						throw IonError{ fmt::format("({}:{}) Bad character", _line, _cursor - _line_base) };

				case Space:
					_cursor = forward_find_if(_cursor + 1, [](char c) { return ::class_of(c) != Space; });
					break;

				case Cr:
					if (_cursor[1] == '\n')
						++_cursor;
					[[fallthrough]];
				case Lf:
					_line_base = _cursor++;
					++_line;
					break;

				case Name:
					if (_stack.back() & AcceptNames)
					{
						const auto begin = _cursor;
						_cursor = forward_find_if(begin + 1, [](char c) { return ::class_of(c) != Name; });
						_stack.back() |= AcceptValues;
						return make_token<IonToken::Type::Name>(begin, _cursor - begin);
					}
					else
						throw IonError{ fmt::format("({}:{}) Unexpected ION name", _line, _cursor - _line_base) };

				case Quote:
					if (_stack.back() & AcceptValues)
					{
						auto cursor = _cursor;
						const auto quote = *cursor;
						const auto base = ++cursor;
						while (*cursor != quote)
						{
							switch (*cursor)
							{
							case '\0':
								throw IonError{ fmt::format("({}:{}) {}", _line, cursor - _line_base, cursor == static_cast<const char*>(_buffer.data()) + _buffer.size() ? "Unexpected end of file" : "Bad character") };
							case '\n':
							case '\r':
								throw IonError{ fmt::format("({}:{}) Unexpected end of line", _line, cursor - _line_base) };
							default:
								++cursor;
							}
						}
						_cursor = cursor + 1;
						return make_token<IonToken::Type::StringValue, -1>(base, cursor - base, quote == '`');
					}
					else
						throw IonError{ fmt::format("({}:{}) Unexpected ION value", _line, _cursor - _line_base) };

				case LBrace:
					if (_stack.back() & AcceptValues)
					{
						_stack.emplace_back(AcceptNames);
						return make_token<IonToken::Type::ObjectBegin>(_cursor++, 1);
					}
					else
						throw IonError{ fmt::format("({}:{}) Unexpected list", _line, _cursor - _line_base) };

				case RBrace:
					if (_stack.back() & AcceptNames && _stack.size() > 1)
					{
						_stack.pop_back();
						return make_token<IonToken::Type::ObjectEnd>(_cursor++, 1);
					}
					else
						throw IonError{ fmt::format("({}:{}) Unexpected end of object", _line, _cursor - _line_base) };

				case LBracket:
					if (_stack.back() & AcceptValues)
					{
						_stack.emplace_back(AcceptValues);
						return make_token<IonToken::Type::ListBegin>(_cursor++, 1);
					}
					else
						throw IonError{ fmt::format("({}:{}) Unexpected list", _line, _cursor - _line_base) };

				case RBracket:
					if ((_stack.back() & (AcceptNames | AcceptValues)) == AcceptValues)
					{
						_stack.pop_back();
						return make_token<IonToken::Type::ListEnd>(_cursor++, 1);
					}
					else
						throw IonError{ fmt::format("({}:{}) Unexpected end of object", _line, _cursor - _line_base) };

				case Comment:
					if (auto next = _cursor + 1; *next == '/')
					{
						_cursor = forward_find_if(next + 1, [](char c) { return c == '\n' || c == '\r' || c == '\0'; });
						break;
					}
					else
						throw IonError{ fmt::format("({}:{}) Bad character", _line, next - _line_base) };
				}
			}
		}

	private:
		template <IonToken::Type type, std::ptrdiff_t column_offset = 0>
		IonToken make_token(const char* begin, std::ptrdiff_t size, bool translatable = false) noexcept
		{
			return { _line, begin - _line_base + column_offset, type, { begin, static_cast<std::size_t>(size) }, translatable };
		}

	private:
		enum : uint8_t
		{
			AcceptNames = 1 << 0,
			AcceptValues = 1 << 1,
		};

		const Buffer _buffer;
		const char* _cursor{ static_cast<const char*>(_buffer.data()) };
		std::size_t _line{ 1 };
		const char* _line_base{ _cursor - 1 };
		std::vector<uint8_t> _stack{ AcceptNames };
	};

	IonReader::IonReader(const Source& source)
		: _private{ std::make_unique<IonReaderPrivate>(source) }
	{
	}

	IonReader::~IonReader() = default;

	IonToken IonReader::read()
	{
		return _private->read();
	}
}
