#ifndef _src_ion_parser_h_
#define _src_ion_parser_h_

#include <yttrium/std/vector.h>

namespace Yttrium
{
	class Buffer;
	class IonDocumentImpl;
	class IonList;
	class IonObject;
	class StaticString;

	class IonParser
	{
	public:
		IonParser(IonDocumentImpl&);

		// The buffer must be zero-terminated.
		bool parse(Buffer&, const StaticString& source_name);

	private:
		bool parse_name(const StaticString& name);
		bool parse_value(const StaticString& value);
		bool parse_lbrace();
		bool parse_rbrace();
		bool parse_lbracket();
		bool parse_rbracket();
		bool parse_end();

	private:
		IonDocumentImpl& _document;

	private:
		enum CharClass
		{
			Other,    // Any character not mentioned below.
			End,      // '\0'.
			Space,    // '\t', '\n', '\v', '\f', '\r' or ' '.
			Name,     // 'A'-'Z', 'a'-'z', '0'-'9' or '_'.
			Quote,    // '"'.
			LBrace,   // '{'.
			RBrace,   // '}'.
			LBracket, // '['.
			RBracket, // ']'.
			Comment,  // '#'.
		};

		static const CharClass char_class[256];

	private:
		struct State
		{
			IonObject* object = nullptr;
			IonList* list = nullptr;

			State(IonObject* object) : object(object) {}
			State(IonList* list) : list(list) {}
		};

		StdVector<State> _states;
		State* _state = nullptr;
	};
}

#endif
