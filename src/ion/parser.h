#ifndef __ION_PARSER_H
#define __ION_PARSER_H

#include <vector>

namespace Yttrium
{
	class IonDocument;
	class IonList;
	class IonObject;
	class StaticString;

	class IonParser
	{
	public:

		IonParser(IonDocument& document) : _document(document) {}

		/// Parse the \a string as an ION document.
		/// \note The \a string MUST have a zero terminator.
		bool parse(const StaticString& string, const StaticString& source_name);

	private:

		bool parse_name(const StaticString& name);
		bool parse_value(const StaticString& value);
		bool parse_lbrace();
		bool parse_rbrace();
		bool parse_lbracket();
		bool parse_rbracket();
		bool parse_end();

	private:

		IonDocument& _document;

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

		std::vector<State> _states;
		State* _state = nullptr;
	};
}

#endif
