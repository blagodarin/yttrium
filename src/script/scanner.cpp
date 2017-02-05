#include "scanner.h"

#include <yttrium/exceptions.h>

namespace
{
	enum class C
	{
		Other,
		Id,
		Digit,
		Sign,
		Space,
		Comment,
		Quote,
		Newline,
		Semicolon,
		End,
		Error,
	};

	const C kind_of[256] =
	{
		C::End,     C::Error,   C::Error,   C::Error,     // 0
		C::Error,   C::Error,   C::Error,   C::Error,     //
		C::Error,   C::Space,   C::Newline, C::Space,     //   t n v
		C::Space,   C::Newline, C::Error,   C::Error,     // f r
		C::Error,   C::Error,   C::Error,   C::Error,     //
		C::Error,   C::Error,   C::Error,   C::Error,     //
		C::Error,   C::Error,   C::Error,   C::Error,     //
		C::Error,   C::Error,   C::Error,   C::Error,     //

		C::Space,   C::Other,   C::Quote,   C::Comment,   //   ! " #
		C::Other,   C::Other,   C::Other,   C::Other,     // $ % & '
		C::Other,   C::Other,   C::Other,   C::Sign,      // ( ) * +
		C::Other,   C::Sign,    C::Other,   C::Other,     // , - . /
		C::Digit,   C::Digit,   C::Digit,   C::Digit,     // 0 1 2 3
		C::Digit,   C::Digit,   C::Digit,   C::Digit,     // 4 5 6 7
		C::Digit,   C::Digit,   C::Other,   C::Semicolon, // 8 9 : ;
		C::Other,   C::Other,   C::Other,   C::Other,     // < = > ?

		C::Other,   C::Id,      C::Id,      C::Id,        // @ A B C
		C::Id,      C::Id,      C::Id,      C::Id,        // D E F G
		C::Id,      C::Id,      C::Id,      C::Id,        // H I J K
		C::Id,      C::Id,      C::Id,      C::Id,        // L M N O
		C::Id,      C::Id,      C::Id,      C::Id,        // P Q R S
		C::Id,      C::Id,      C::Id,      C::Id,        // T U V W
		C::Id,      C::Id,      C::Id,      C::Other,     // X Y Z [
		C::Other,   C::Other,   C::Other,   C::Id,        // \ ] ^ _

		C::Other,   C::Id,      C::Id,      C::Id,        // ` a b c
		C::Id,      C::Id,      C::Id,      C::Id,        // d e f g
		C::Id,      C::Id,      C::Id,      C::Id,        // h i j k
		C::Id,      C::Id,      C::Id,      C::Id,        // l m n o
		C::Id,      C::Id,      C::Id,      C::Id,        // p q r s
		C::Id,      C::Id,      C::Id,      C::Id,        // t u v w
		C::Id,      C::Id,      C::Id,      C::Other,     // x y z {
		C::Other,   C::Other,   C::Other,   C::Error,     // | } ~

		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //

		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //

		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //

		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
		C::Other,   C::Other,   C::Other,   C::Other,     //
	};
}

namespace Yttrium
{
	ScriptScanner::ScriptScanner(const StaticString& text)
		: _cursor(const_cast<char*>(text.text()))
		, _end(text.text() + text.size())
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
			case C::Id:
				{
					const auto begin = _cursor;
					do ++_cursor; while (::kind_of[static_cast<unsigned char>(*_cursor)] == C::Id || ::kind_of[static_cast<unsigned char>(*_cursor)] == C::Digit);
					token.string = StaticString(begin, _cursor - begin);
				}
				token.type = Token::Identifier;
				return token;

			case C::Sign:
				has_sign = true;
				++_cursor;
				if (::kind_of[static_cast<unsigned char>(*_cursor)] != C::Digit)
					throw DataError("[", _line, ":", _cursor - _line_origin, "] '+' or '-' must be followed by a digit");
			case C::Digit:
				{
					const auto begin = has_sign ? _cursor - 1 : _cursor;
					do ++_cursor; while (::kind_of[static_cast<unsigned char>(*_cursor)] == C::Digit);
					token.string = StaticString(begin, _cursor - begin);
				}
				token.type = Token::Number;
				return token;

			case C::Space:
				do ++_cursor; while (::kind_of[static_cast<unsigned char>(*_cursor)] == C::Space);
				break;

			case C::Comment:
				do ++_cursor; while (*_cursor != '\n' && *_cursor != '\r' && _cursor != _end);
				break;

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
							default: throw DataError("[", _line, ":", _cursor - _line_origin, "] Invalid escape character");
							}
							++_cursor;
						}
						else if (*_cursor == '\n' || *_cursor == '\r')
						{
							throw DataError("[", _line, ":", _cursor - _line_origin, "] Unexpected end of line");
						}
						else if (_cursor != _end)
						{
							if (end != _cursor)
								*end = *_cursor;
							++_cursor;
							++end;
						}
						else
							throw DataError("[", _line, ":", _cursor - _line_origin, "] Unexpected end of file");
					}
					token.string = StaticString(begin, end - begin);
				}
				token.type = Token::String;
				return token;

			case C::Newline:
				if (*_cursor == '\r' && *(_cursor + 1) == '\n')
					++_cursor;
				++_line;
				_line_origin = _cursor++;
				token.type = Token::Separator;
				return token;

			case C::Semicolon:
				++_cursor;
				token.type = Token::Separator;
				return token;

			case C::End:
				if (_cursor != _end)
				{
					++_cursor;
					break;
				}
				token.type = Token::End;
				return token;

			default:
				throw DataError("[", _line, ":", _cursor - _line_origin, "] Invalid character");
			}
		}
	}
}
