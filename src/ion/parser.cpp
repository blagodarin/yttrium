#include "parser.h"

#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/memory/buffer.h>
#include "document.h"

#include <array>
#include <cassert>

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
		Quote,    // '"'.
		LBrace,   // '{'.
		RBrace,   // '}'.
		LBracket, // '['.
		RBracket, // ']'.
		Comment,  // '#'.
	};

	const std::array<CharClass, 256> char_class =
	{
		// #0 - #31: Special character set.

		End,    Other,  Other, Other,    Other, Other,    Other, Other, // \0
		Other,  Space,  Lf,    Space,    Space, Cr,       Other, Other, // \t \n \v \f \r
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

	CharClass class_of(char c)
	{
		return char_class[static_cast<unsigned char>(c)];
	}

	struct State
	{
		Yttrium::IonObject* object = nullptr;
		Yttrium::IonList* list = nullptr;

		State() = default;
		explicit State(Yttrium::IonObject* object_) : object{object_} {}
		explicit State(Yttrium::IonList* list_) : list{list_} {}
	};
}

namespace Yttrium
{
	IonParser::Result IonParser::parse(IonDocumentImpl& document)
	{
		assert(document._buffer.capacity() > document._buffer.size() && document._buffer[document._buffer.size()] == '\0');

		std::array<State, 64> states;
		auto state = states.begin();
		*state = State(&document.root());

		size_t i = 0;
		auto line_base = static_cast<size_t>(-1);
		size_t line = 1;

		for (const auto c = reinterpret_cast<char*>(document._buffer.begin());;)
		{
			switch (::class_of(c[i]))
			{
			case Other:
				return { Status::Error, line, i - line_base };

			case End:
				if (i == document._buffer.size())
					return { state == states.begin() ? Status::Ok : Status::Error, line, i - line_base };

			case Space:
				do { ++i; } while (::class_of(c[i]) == Space);
				break;

			case Cr:
				if (c[i + 1] == '\n')
					++i;

			case Lf:
				line_base = i++;
				++line;
				break;

			case Name:
				if (!state->object)
					return { Status::Error, line, i - line_base };
				{
					const auto base = i;
					do { ++i; } while (::class_of(c[i]) == Name);
					state->list = state->object->append(StaticString(&c[base], i - base), ByReference());
				}
				break;

			case Quote:
				if (!state->list)
					return { Status::Error, line, i - line_base };
				{
					const auto base = ++i;
					while (c[i] != '"')
					{
						switch (c[i])
						{
						case '\0':
							if (i == document._buffer.size())
								return { Status::Error, line, i - line_base };
							++i;
							break;
						case '\n':
						case '\r':
							return { Status::Error, line, i - line_base };
						default:
							++i;
						}
					}
					state->list->append(StaticString(&c[base], i - base), ByReference());
					++i;
				}
				break;

			case LBrace:
				if (!state->list)
					return { Status::Error, line, i - line_base };
				{
					const auto next_state = std::next(state);
					if (next_state == states.end())
						return { Status::Error, line, i - line_base };
					*next_state = State(state->list->append_object());
					state = next_state;
				}
				++i;
				break;

			case RBrace:
				if (!state->object || state == states.begin())
					return { Status::Error, line, i - line_base };
				--state;
				++i;
				break;

			case LBracket:
				if (!state->list)
					return { Status::Error, line, i - line_base };
				{
					const auto next_state = std::next(state);
					if (next_state == states.end())
						return { Status::Error, line, i - line_base };
					*next_state = State(state->list->append_list());
					state = next_state;
				}
				++i;
				break;

			case RBracket:
				if (state->object || state == states.begin())
					return { Status::Error, line, i - line_base };
				--state;
				++i;
				break;

			case Comment:
				do { ++i; } while (c[i] != '\n' && c[i] != '\r' && c[i]);
				break;
			}
		}
	}
}
