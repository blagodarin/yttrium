#include "parser.hpp"

namespace Yttrium
{

namespace Ion
{

Parser::Parser(/*Document& document, */Allocator *allocator)
	: _allocator(allocator)
	, _logger("yttrium.ion.parser", allocator)
//	, _document(document)
{
}

bool Parser::parse(const StaticString& string)
{
	Y_TRACE(_logger, "Parsing...");

//	_objects.push_back(Object());
//	_states.push_back(State(&_objects.back()));
//	_state = &_states.back();

	for (const char *src = string.text(); ; )
	{
		switch (char_class[static_cast<UChar>(*src)])
		{
		case Other:

			return false;

		case End:

			return (src == string.text() + string.size())
				&& parse_end();

		case Space:

			do
			{
				++src;
			} while (char_class[static_cast<UChar>(*src)] == Space);
			break;

		case Name:

			{
				const char *begin = src;

				do
				{
					++src;
				} while (char_class[static_cast<UChar>(*src)] == Name);

				if (!parse_name(StaticString(begin, src - begin)))
				{
					return false;
				}
			}
			break;

		case Quote:

			{
				const char *begin = ++src;

				while (*src != '"' && *src != '\\' && *src)
				{
					++src;
				}

				char *dst = const_cast<char *>(src);

				while (*src != '"')
				{
					if (!*src)
					{
						return false;
					}
					else if (*src == '\\')
					{
						char c1 = *++src;

						if ((c1 >= '0' && c1 <= '9') || (c1 >= 'A' && c1 <= 'F'))
						{
							UChar hex;

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
								return false;
							}

							if (c1 <= '9')
							{
								hex += 16 * (c1 - '9');
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
							case 'n':  *dst++ = '\n'; break; // NOTE: It can be easily fixed to say "\r\n".
							case 'r':  *dst++ = '\r'; break;
							case 't':  *dst++ = '\t'; break;
							default:   return false;
							}
						}
					}
					else
					{
						*dst++ = *src++;
					}
				}
				if (!parse_value(StaticString(begin, dst - begin)))
				{
					return false;
				}
				++src;
			}
			break;

		case LBrace:

			if (!parse_lbrace())
			{
				return false;
			}
			++src;
			break;

		case RBrace:

			if (!parse_rbrace())
			{
				return false;
			}
			++src;
			break;

		case LBracket:

			if (!parse_lbracket())
			{
				return false;
			}
			++src;
			break;

		case RBracket:

			if (!parse_rbracket())
			{
				return false;
			}
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

bool Parser::parse_name(const StaticString& name)
{
	Y_TRACE(_logger, "Token: " << name);

//	if (!_state->object)
//		return false;
//	_state->list = _state->object->add_node(name);
	return true;
}

bool Parser::parse_value(const StaticString& value)
{
	Y_TRACE(_logger, "Token: \"" << String(value, String::Reference, _allocator).escaped("\\\"", '\\') << "\"");

//	if (!_state->list)
//		return false;
//	_state->list->add_value(value);
	return true;
}

bool Parser::parse_lbrace()
{
	Y_TRACE(_logger, "Token: {");

//	if (!_state->list)
//		return false;
//	_states.push_back(State(_state->list->add_object()));
//	_state = &_states.back();
	return true;
}

bool Parser::parse_rbrace()
{
	Y_TRACE(_logger, "Token: }");

//	if (!_state->object || _states.size() == 1)
//		return false;
//	_states.pop_back();
//	_state = &_states.back();
	return true;
}

bool Parser::parse_lbracket()
{
	Y_TRACE(_logger, "Token: [");

//	if (!_state->list)
//		return false;
//	_states.push_back(State(_state->list->add_list()));
//	_state = &_states.back();
	return true;
}

bool Parser::parse_rbracket()
{
	Y_TRACE(_logger, "Token: ]");

//	if (_state->object || _states.size() == 1)
//		return false;
//	_states.pop_back();
//	_state = &_states.back();
	return true;
}

bool Parser::parse_end()
{
	//FALX_LOG_TRACE(_logger, "Token: <end>");

//	return _states.size() == 1;
	return true;
}

const Parser::CharClass Parser::char_class[256] =
{
	// #0 - #31: Special character set.

	End,    Other,  Other, Other,   Other, Other,  Other, Other, // \0
	Other,  Space,  Space, Space,   Space, Space,  Other, Other, // \t \n \v \f \r
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //

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

	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
	Other,  Other,  Other, Other,   Other, Other,  Other, Other, //
};

} // namespace Ion

} // namespace Yttrium
