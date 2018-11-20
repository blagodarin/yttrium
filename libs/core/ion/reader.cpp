//
// Copyright 2018 Sergei Blagodarin
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

#include <yttrium/ion/reader.h>

#include <yttrium/math/color.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/storage/source.h>
#include "../utils/algorithm.h"

#include <array>
#include <vector>

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
		Hash,     // '#'.
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
		Space, Other, Quote, Hash, Other, Other, Other, Other,    //   ! " # $ % & '
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
}

namespace Yttrium
{
	void IonReader::Token::check_end() const
	{
		if (_type != Type::End)
			throw IonError{ _line, _column, "End of file expected" };
	}

	void IonReader::Token::check_list_begin() const
	{
		if (_type != Type::ListBegin)
			throw IonError{ _line, _column, "'[' expected" };
	}

	void IonReader::Token::check_list_end() const
	{
		if (_type != Type::ListEnd)
			throw IonError{ _line, _column, "']' expected" };
	}

	void IonReader::Token::check_name(std::string_view name) const
	{
		if (to_name() != name)
			throw IonError{ _line, _column, "'", name, "' expected" };
	}

	void IonReader::Token::check_object_begin() const
	{
		if (_type != Type::ObjectBegin)
			throw IonError{ _line, _column, "'{' expected" };
	}

	void IonReader::Token::check_object_end() const
	{
		if (_type != Type::ObjectEnd)
			throw IonError{ _line, _column, "'}' expected" };
	}

	IonReader::Token& IonReader::Token::next(IonReader& ion)
	{
		return *this = ion.read();
	}

	Color4f IonReader::Token::to_color() const
	{
		if (_type != Type::Color)
			throw IonError{ _line, _column, "ION color expected" };

		const auto d = [this](std::size_t i) {
			const auto c = _text[i];
			return static_cast<float>(c < 'a' ? c - '0' : c - 'a' + 10);
		};

		switch (_text.size())
		{
		case 4: return { d(1) / 15.f, d(2) / 15.f, d(3) / 15.f };
		case 5: return { d(1) / 15.f, d(2) / 15.f, d(3) / 15.f, d(4) / 15.f };
		case 7: return { (d(1) * 16 + d(2)) / 255.f, (d(3) * 16 + d(4)) / 255.f, (d(5) * 16 + d(6)) / 255.f };
		case 9: return { (d(1) * 16 + d(2)) / 255.f, (d(3) * 16 + d(4)) / 255.f, (d(5) * 16 + d(6)) / 255.f, (d(7) * 16 + d(8)) / 255.f };
		default: throw IonError{ _line, _column, "Bad ION color" };
		}
	}

	std::string_view IonReader::Token::to_name() const
	{
		if (_type != Type::Name)
			throw IonError{ _line, _column, "ION name expected" };
		return _text;
	}

	std::string_view IonReader::Token::to_value() const
	{
		if (_type != Type::Value)
			throw IonError{ _line, _column, "ION value expected" };
		return _text;
	}

	class IonReaderPrivate
	{
	public:
		explicit IonReaderPrivate(const Source& source)
			: _buffer{ source.to_buffer() }
		{
		}

		IonReader::Token read()
		{
			for (;;)
			{
				switch (::class_of(*_cursor))
				{
				case Other:
					throw IonError{ _line, _cursor - _line_base, "Bad character" };

				case End:
					if (_cursor == static_cast<const char*>(_buffer.data()) + _buffer.size())
						return make_token<IonReader::Token::Type::End>(_cursor, 0);
					else
						throw IonError{ _line, _cursor - _line_base, "Bad character" };

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
						return make_token<IonReader::Token::Type::Name>(begin, _cursor - begin);
					}
					else
						throw IonError{ _line, _cursor - _line_base, "Unexpected ION name" };

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
								throw IonError{ _line, cursor - _line_base, cursor == static_cast<const char*>(_buffer.data()) + _buffer.size() ? "Unexpected end of file" : "Bad character" };
							case '\n':
							case '\r':
								throw IonError{ _line, cursor - _line_base, "Unexpected end of line" };
							default:
								++cursor;
							}
						}
						_cursor = cursor + 1;
						return make_token<IonReader::Token::Type::Value, -1>(base, cursor - base, quote == '`');
					}
					else
						throw IonError{ _line, _cursor - _line_base, "Unexpected ION value" };

				case Hash:
					if (_stack.back() & AcceptValues)
					{
						const auto begin = _cursor;
						const auto end = forward_find_if(begin + 1, [](char c) { return (c < '0' || c > '9') && (c < 'a' || c > 'f'); });
						if (const auto next_class = ::class_of(*end); next_class == Other || next_class == Name)
							throw IonError{ _line, end - _line_base, "Bad character" };
						_cursor = end;
						return make_token<IonReader::Token::Type::Color>(begin, end - begin);
					}
					else
						throw IonError{ _line, _cursor - _line_base, "Unexpected ION value" };

				case LBrace:
					if (_stack.back() & AcceptValues)
					{
						_stack.emplace_back(AcceptNames);
						return make_token<IonReader::Token::Type::ObjectBegin>(_cursor++, 1);
					}
					else
						throw IonError{ _line, _cursor - _line_base, "Unexpected list" };

				case RBrace:
					if (_stack.back() & AcceptNames && _stack.size() > 1)
					{
						_stack.pop_back();
						return make_token<IonReader::Token::Type::ObjectEnd>(_cursor++, 1);
					}
					else
						throw IonError{ _line, _cursor - _line_base, "Unexpected end of object" };

				case LBracket:
					if (_stack.back() & AcceptValues)
					{
						_stack.emplace_back(AcceptValues);
						return make_token<IonReader::Token::Type::ListBegin>(_cursor++, 1);
					}
					else
						throw IonError{ _line, _cursor - _line_base, "Unexpected list" };

				case RBracket:
					if ((_stack.back() & (AcceptNames | AcceptValues)) == AcceptValues)
					{
						_stack.pop_back();
						return make_token<IonReader::Token::Type::ListEnd>(_cursor++, 1);
					}
					else
						throw IonError{ _line, _cursor - _line_base, "Unexpected end of object" };

				case Comment:
					if (auto next = _cursor + 1; *next == '/')
					{
						_cursor = forward_find_if(next + 1, [](char c) { return c == '\n' || c == '\r' || c == '\0'; });
						break;
					}
					else
						throw IonError{ _line, next - _line_base, "Bad character" };
				}
			}
		}

	private:
		template <IonReader::Token::Type type, std::ptrdiff_t column_offset = 0>
		IonReader::Token make_token(const char* begin, std::ptrdiff_t size, bool translatable = false) noexcept
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

	IonReader::Token IonReader::read()
	{
		return _private->read();
	}
}
