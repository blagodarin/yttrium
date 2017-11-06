#include "scanner.h"

#include <yttrium/exceptions.h>

namespace
{
	enum class C
	{
		_,
		Comment,
		Digit,
		End,
		Equals,
		Id,
		Newline,
		Quote,
		Semicolon,
		Sign,
		Space,
	};

	const C kind_of[256] =
	{
		C::End,   C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     // 0
		C::_,     C::Space,  C::Newline, C::Space,     C::Space, C::Newline, C::_,     C::_,     //   t n v f r
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::Space, C::_,      C::Quote,   C::Comment,   C::_,     C::_,       C::_,     C::_,     //   ! " # $ % & '
		C::_,     C::_,      C::_,       C::Sign,      C::_,     C::Sign,    C::_,     C::_,     // ( ) * + , - . /
		C::Digit, C::Digit,  C::Digit,   C::Digit,     C::Digit, C::Digit,   C::Digit, C::Digit, // 0 1 2 3 4 5 6 7
		C::Digit, C::Digit,  C::_,       C::Semicolon, C::_,     C::Equals,  C::_,     C::_,     // 8 9 : ; < = > ?
		C::_,     C::Id,     C::Id,      C::Id,        C::Id,    C::Id,      C::Id,    C::Id,    // @ A B C D E F G
		C::Id,    C::Id,     C::Id,      C::Id,        C::Id,    C::Id,      C::Id,    C::Id,    // H I J K L M N O
		C::Id,    C::Id,     C::Id,      C::Id,        C::Id,    C::Id,      C::Id,    C::Id,    // P Q R S T U V W
		C::Id,    C::Id,     C::Id,      C::_,         C::_,     C::_,       C::_,     C::Id,    // X Y Z [ \ ] ^ _
		C::_,     C::Id,     C::Id,      C::Id,        C::Id,    C::Id,      C::Id,    C::Id,    // ` a b c d e f g
		C::Id,    C::Id,     C::Id,      C::Id,        C::Id,    C::Id,      C::Id,    C::Id,    // h i j k l m n o
		C::Id,    C::Id,     C::Id,      C::Id,        C::Id,    C::Id,      C::Id,    C::Id,    // p q r s t u v w
		C::Id,    C::Id,     C::Id,      C::_,         C::_,     C::_,       C::_,     C::_,     // x y z { | } ~
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
		C::_,     C::_,      C::_,       C::_,         C::_,     C::_,       C::_,     C::_,     //
	};
}

namespace Yttrium
{
	ScriptScanner::ScriptScanner(std::string& text)
		: _cursor{text.data()}
		, _end{text.data() + text.size()}
	{
	}

	ScriptScanner::Token ScriptScanner::read()
	{
		Token token;
		token.line = _line;
		for (;;)
		{
			bool has_sign = false;
			token.column = _cursor - _line_origin;
			switch (::kind_of[static_cast<unsigned char>(*_cursor)])
			{
			case C::Comment:
				do ++_cursor; while (*_cursor != '\n' && *_cursor != '\r' && _cursor != _end);
				break;

			case C::Id:
				{
					const auto begin = _cursor;
					do ++_cursor; while (::kind_of[static_cast<unsigned char>(*_cursor)] == C::Id || ::kind_of[static_cast<unsigned char>(*_cursor)] == C::Digit);
					token.string = {begin, static_cast<size_t>(_cursor - begin)};
				}
				token.type = Token::Identifier;
				return token;

			case C::Sign:
				has_sign = true;
				++_cursor;
				if (::kind_of[static_cast<unsigned char>(*_cursor)] != C::Digit)
					throw DataError{"[", _line, ":", _cursor - _line_origin, "] '+' or '-' must be followed by a digit"};
				[[fallthrough]];
			case C::Digit:
				{
					const auto begin = has_sign ? _cursor - 1 : _cursor;
					do ++_cursor; while (::kind_of[static_cast<unsigned char>(*_cursor)] == C::Digit);
					token.string = {begin, static_cast<size_t>(_cursor - begin)};
				}
				token.type = Token::Number;
				return token;

			case C::End:
				if (_cursor != _end)
				{
					++_cursor;
					break;
				}
				token.type = Token::End;
				return token;

			case C::Equals:
				++_cursor;
				token.type = Token::Equals;
				return token;

			case C::Newline:
				if (*_cursor == '\r' && *(_cursor + 1) == '\n')
					++_cursor;
				++_line;
				_line_origin = _cursor++;
				token.type = Token::Separator;
				return token;

			case C::Quote:
				{
					const auto quote = *_cursor;
					const auto begin = ++_cursor;
					auto end = begin;
					for (;;)
					{
						if (*_cursor == quote)
						{
							++_cursor;
							break;
						}
						else if (*_cursor == '\\')
						{
							switch (*++_cursor)
							{
							case '\\': *end++ = '\\'; break;
							case '"': *end++ = '"'; break;
							case '\'': *end++ = '\''; break;
							case 'n': *end++ = '\n'; break;
							case 'r': *end++ = '\r'; break;
							default: throw DataError{"[", _line, ":", _cursor - _line_origin, "] Invalid escape character"};
							}
							++_cursor;
						}
						else if (*_cursor == '\n' || *_cursor == '\r')
						{
							throw DataError{"[", _line, ":", _cursor - _line_origin, "] Unexpected end of line"};
						}
						else if (_cursor != _end)
						{
							if (end != _cursor)
								*end = *_cursor;
							++_cursor;
							++end;
						}
						else
							throw DataError{"[", _line, ":", _cursor - _line_origin, "] Unexpected end of file"};
					}
					token.string = {begin, static_cast<size_t>(end - begin)};
				}
				token.type = Token::String;
				return token;

			case C::Semicolon:
				++_cursor;
				token.type = Token::Separator;
				return token;

			case C::Space:
				do ++_cursor; while (::kind_of[static_cast<unsigned char>(*_cursor)] == C::Space);
				break;

			default:
				throw DataError{"[", _line, ":", _cursor - _line_origin, "] Invalid character"};
			}
		}
	}
}
