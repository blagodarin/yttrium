#include "scanner.h"

#include <Yttrium/assert.h>

#include "logging.h"

namespace Yttrium
{

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

} // namespace

ScriptScanner::ScriptScanner(const StaticString &text)
	: _cursor(const_cast<char *>(text.text()))
	, _end(text.text() + text.size())
	, _line_origin(_cursor - 1)
	, _line(1)
{
}

bool ScriptScanner::read(Token *token)
{
	char *cursor = _cursor;

	token->line = _line;

rescan:

	switch (kind_of[static_cast<UChar>(*cursor)])
	{
	case Kind::Other:

		{
			token->column = cursor - _line_origin;
			token->type = Token::Literal;

			char *begin = cursor;

			bool has_sigil = (*cursor == '+' || *cursor == '-');

			if (has_sigil)
			{
				++cursor;
			}

			if ((*cursor >= 'a' && *cursor <= 'z') || *cursor == '_'
				|| (*cursor >= 'A' && *cursor <= 'Z'))
			{
				token->type = Token::Identifier;
				while ((*cursor >= 'a' && *cursor <= 'z') || *cursor == '_'
					|| (*cursor >= '0' && *cursor <= '9') || (*cursor >= 'A' && *cursor <= 'Z'))
				{
					++cursor;
				}
			}

			if (kind_of[static_cast<UChar>(*cursor)] == Kind::Other)
			{
				token->type = Token::Literal;
				do ++cursor; while (kind_of[static_cast<UChar>(*cursor)] == Kind::Other);
			}

			if (token->type == Token::Identifier && has_sigil)
			{
				token->type = Token::XIdentifier;
			}

			token->string = StaticString(begin, cursor - begin);
		}
		break;

	case Kind::Space:

		do ++cursor; while (kind_of[static_cast<UChar>(*cursor)] == Kind::Space);
		goto rescan;

	case Kind::Comment:

		do ++cursor; while (cursor != _end && *cursor != '\r' && *cursor != '\n');
		goto rescan;

	case Kind::Quote:

		{
			token->column = cursor - _line_origin;

			char *begin = ++cursor;
			char *end   = begin;

			for (; ; )
			{
				if (*cursor == '"')
				{
					++cursor;
					break;
				}
				else if (*cursor == '\\')
				{
					switch (*++cursor)
					{
					case '\\': *end++ = '\\'; break;
					case '"':  *end++ = '"';  break;
					case '\'': *end++ = '\''; break;
					case 'n':  *end++ = '\n'; break;
					case 'r':  *end++ = '\r'; break;
					default:

						Y_LOG(S("[ScriptScanner] Invalid escape sequence \"") << *cursor
							<< S("\" (") << token->line << ':' << cursor - _line_origin << ')');
						return false;
					}
					++cursor;
				}
				else if (*cursor == '\n' || *cursor == '\r')
				{
					Y_LOG(S("[ScriptScanner] Unexpected end of line (")
						<< token->line << ':' << cursor - _line_origin << ')');
					return false;
				}
				else if (cursor != _end)
				{
					if (end != cursor)
					{
						*end = *cursor;
					}
					++cursor;
					++end;
				}
				else
				{
					Y_LOG(S("[ScriptScanner] Unexpected end of script (")
						<< token->line << ':' << cursor - _line_origin << ')');
					return false;
				}
			}

			token->type = Token::String;
			token->string = StaticString(begin, end - begin);
		}
		break;

	case Kind::Newline:

		token->column = cursor - _line_origin;
		if (*cursor == '\r' && *cursor == '\n') // Treat "\r\n" as a single newline.
		{
			++cursor;
		}
		++_line;
		_line_origin = cursor - 1;
		token->type = Token::Separator;
		break;

	case Kind::Semicolon:

		token->column = cursor++ - _line_origin;
		token->type = Token::Separator;
		break;

	case Kind::End:

		if (cursor != _end)
		{
			++cursor;
			goto rescan;
		}
		token->column = cursor - _line_origin;
		token->type = Token::End;
		break;

	case Kind::Error:

		Y_LOG(S("[ScriptScanner] Invalid script character (")
			<< token->line << ':' << cursor - _line_origin << ')');
		return false;
	}

	_cursor = cursor;

	return true;
}

} // namespace Yttrium
