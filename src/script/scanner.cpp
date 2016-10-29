#include "scanner.h"

#include <yttrium/exceptions.h>

namespace
{
	enum class Kind
	{
		Other,
		Space,
		Comment,
		Quote,
		Newline,
		Semicolon,
		End,
		Error,
	};

	const Kind kind_of[256] =
	{
		Kind::End,     Kind::Error,   Kind::Error,   Kind::Error,     // 0
		Kind::Error,   Kind::Error,   Kind::Error,   Kind::Error,     //
		Kind::Error,   Kind::Space,   Kind::Newline, Kind::Space,     //   t n v
		Kind::Space,   Kind::Newline, Kind::Error,   Kind::Error,     // f r
		Kind::Error,   Kind::Error,   Kind::Error,   Kind::Error,     //
		Kind::Error,   Kind::Error,   Kind::Error,   Kind::Error,     //
		Kind::Error,   Kind::Error,   Kind::Error,   Kind::Error,     //
		Kind::Error,   Kind::Error,   Kind::Error,   Kind::Error,     //

		Kind::Space,   Kind::Other,   Kind::Quote,   Kind::Comment,   //   ! " #
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // $ % & '
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // ( ) * +
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // , - . /
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // 0 1 2 3
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // 4 5 6 7
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Semicolon, // 8 9 : ;
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // < = > ?

		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // @ A B C
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // D E F G
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // H I J K
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // L M N O
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // P Q R S
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // T U V W
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // X Y Z [
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // \ ] ^ _

		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // ` a b c
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // d e f g
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // h i j k
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // l m n o
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // p q r s
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // t u v w
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     // x y z {
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Error,     // | } ~

		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //

		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //

		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //

		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
		Kind::Other,   Kind::Other,   Kind::Other,   Kind::Other,     //
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
			token.column = _cursor - _line_origin;
			switch (::kind_of[static_cast<unsigned char>(*_cursor)])
			{
			case Kind::Other:
				token.type = Token::Literal;
				{
					char* begin = _cursor;

					const bool has_sigil = (*_cursor == '+' || *_cursor == '-');
					if (has_sigil)
						++_cursor;

					if ((*_cursor >= 'a' && *_cursor <= 'z') || *_cursor == '_' || (*_cursor >= 'A' && *_cursor <= 'Z'))
					{
						token.type = Token::Identifier;
						while ((*_cursor >= 'a' && *_cursor <= 'z') || *_cursor == '_'
							|| (*_cursor >= '0' && *_cursor <= '9') || (*_cursor >= 'A' && *_cursor <= 'Z'))
						{
							++_cursor;
						}
					}

					if (::kind_of[static_cast<unsigned char>(*_cursor)] == Kind::Other)
					{
						token.type = Token::Literal;
						do ++_cursor; while (::kind_of[static_cast<unsigned char>(*_cursor)] == Kind::Other);
					}

					if (token.type == Token::Identifier && has_sigil)
						token.type = Token::XIdentifier;

					token.string = StaticString(begin, _cursor - begin);
				}
				return token;

			case Kind::Space:
				do ++_cursor; while (::kind_of[static_cast<unsigned char>(*_cursor)] == Kind::Space);
				break;

			case Kind::Comment:
				do ++_cursor; while (*_cursor != '\n' && *_cursor != '\r' && _cursor != _end);
				break;

			case Kind::Quote:
				{
					auto begin = ++_cursor;
					auto end = begin;
					for (;;)
					{
						if (*_cursor == '"')
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

			case Kind::Newline:
				if (*_cursor == '\r' && *(_cursor + 1) == '\n')
					++_cursor;
				++_line;
				_line_origin = _cursor++;
				token.type = Token::Separator;
				return token;

			case Kind::Semicolon:
				++_cursor;
				token.type = Token::Separator;
				return token;

			case Kind::End:
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
