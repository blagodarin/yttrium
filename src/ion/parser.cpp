#include "parser.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/log.h>

namespace Yttrium
{
	IonParser::IonParser(IonDocument* document)
		: _document(document)
	{
	}

	bool IonParser::parse(const StaticString& string, const StaticString& source_name)
	{
		_states.push_back(State(_document));
		_state = &_states.back();

		for (const char* src = string.text(); ; )
		{
			switch (char_class[static_cast<unsigned char>(*src)])
			{
			case Other:
				return false;

			case End:
				return src == string.text() + string.size()
					&& parse_end();

			case Space:
				do
				{
					++src;
				} while (char_class[static_cast<unsigned char>(*src)] == Space);
				break;

			case Name:
				{
					const char* begin = src;

					do
					{
						++src;
					} while (char_class[static_cast<unsigned char>(*src)] == Name);

					if (!parse_name(StaticString(begin, src - begin)))
					{
						return false;
					}
				}
				break;

			case Quote:
				{
					const char* begin = ++src;

					while (*src != '"' && *src != '\\' && *src)
						++src;

					char* dst = const_cast<char*>(src);

					while (*src != '"')
					{
						char c0 = *src;

						if (!c0 && src == string.text() + string.size())
						{
							Y_LOG(S("ion: Error: ") << source_name << S(": String continues past the end of source"));
							return false;
						}

						if (c0 == '\\')
						{
							char c1 = *++src;

							if ((c1 >= '0' && c1 <= '9') || (c1 >= 'A' && c1 <= 'F'))
							{
								unsigned char hex;

								char c2 = *++src;

								if (c2 >= '0' && c2 <= '9')
								{
									hex = c2 - '0';
								}
								else if (c2 >= 'A' && c2 <= 'F')
								{
									hex = c2 - 'A' + 10;
								}
								else
								{
									Y_LOG(S("ion: Error: ") << source_name << S(": Bad second hex digit '") << c2 << '\'');
									return false;
								}

								if (c1 <= '9')
								{
									hex += 16 * (c1 - '0');
								}
								else
								{
									hex += 16 * (c1 - 'A' + 10);
								}

								*dst++ = hex;
							}
							else
							{
								switch (c1)
								{
								case '"':  *dst++ = '\"'; break;
								case '\\': *dst++ = '\\'; break;
								case 'n':  *dst++ = '\n'; break;
								case 'r':  *dst++ = '\r'; break;
								case 't':  *dst++ = '\t'; break;
								default:

									Y_LOG(S("ion: Error: ") << source_name << S(": Bad escape symbol '") << c1 << '\'');
									return false;
								}
							}
						}
						else
						{
							*dst++ = c0;
						}

						++src;
					}
					if (!parse_value(StaticString(begin, dst - begin)))
						return false;
					++src;
				}
				break;

			case LBrace:
				if (!parse_lbrace())
					return false;
				++src;
				break;

			case RBrace:
				if (!parse_rbrace())
					return false;
				++src;
				break;

			case LBracket:
				if (!parse_lbracket())
					return false;
				++src;
				break;

			case RBracket:
				if (!parse_rbracket())
					return false;
				++src;
				break;

			case Comment:
				do
				{
					++src;
				} while (*src != '\n' && *src != '\r' && *src);
				break;
			}
		}
	}

	bool IonParser::parse_name(const StaticString& name)
	{
		if (!_state->object)
			return false;
		_state->list = _state->object->append(name, ByReference());
		return true;
	}

	bool IonParser::parse_value(const StaticString& value)
	{
		if (!_state->list)
			return false;
		_state->list->append(value, ByReference());
		return true;
	}

	bool IonParser::parse_lbrace()
	{
		if (!_state->list)
			return false;
		_states.emplace_back(_state->list->append_object());
		_state = &_states.back();
		return true;
	}

	bool IonParser::parse_rbrace()
	{
		if (!_state->object || _states.size() == 1)
			return false;
		_states.pop_back();
		_state = &_states.back();
		return true;
	}

	bool IonParser::parse_lbracket()
	{
		if (!_state->list)
			return false;
		_states.emplace_back(_state->list->append_list());
		_state = &_states.back();
		return true;
	}

	bool IonParser::parse_rbracket()
	{
		if (_state->object || _states.size() == 1)
			return false;
		_states.pop_back();
		_state = &_states.back();
		return true;
	}

	bool IonParser::parse_end()
	{
		return _states.size() == 1;
	}

	const IonParser::CharClass IonParser::char_class[256] =
	{
		// #0 - #31: Special character set.

		End,    Other,  Other, Other,    Other, Other,    Other, Other, // \0
		Other,  Space,  Space, Space,    Space, Space,    Other, Other, // \t \n \v \f \r
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //

		// #32 - #127: Basic character set.

		Space,  Other,  Quote, Comment,  Other, Other,    Other, Other, //   ! " # $ % & '
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, // ( ) * + , - . /
		Name,   Name,   Name,  Name,     Name,  Name,     Name,  Name,  // 0 1 2 3 4 5 6 7
		Name,   Name,   Other, Other,    Other, Other,    Other, Other, // 8 9 : ; < = > ?
		Other,  Name,   Name,  Name,     Name,  Name,     Name,  Name,  // @ A B C D E F G
		Name,   Name,   Name,  Name,     Name,  Name,     Name,  Name,  // H I J K L M N O
		Name,   Name,   Name,  Name,     Name,  Name,     Name,  Name,  // P Q R S T U V W
		Name,   Name,   Name,  LBracket, Other, RBracket, Other, Name,  // X Y Z [ \ ] ^ _
		Other,  Name,   Name,  Name,     Name,  Name,     Name,  Name,  // ` a b c d e f g
		Name,   Name,   Name,  Name,     Name,  Name,     Name,  Name,  // h i j k l m n o
		Name,   Name,   Name,  Name,     Name,  Name,     Name,  Name,  // p q r s t u v w
		Name,   Name,   Name,  LBrace,   Other, RBrace,   Other, Other, // x y z { | } ~

		// #128 - #255: Extended character set.

		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
		Other,  Other,  Other, Other,    Other, Other,    Other, Other, //
	};
}
