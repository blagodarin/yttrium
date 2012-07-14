#ifndef __ION_PARSER_H
#define __ION_PARSER_H

#include <Yttrium/allocator.h>
#include <Yttrium/ion/document.h>
#include <Yttrium/static_string.h>

#include <vector>

namespace Yttrium
{

namespace Ion
{

class Parser
{
public:

	Parser(Document *document);

	/// Parse the \a string as an ION document.
	/// \note The \a string MUST have a zero terminator.

	bool parse(const StaticString &string, const StaticString &source_name);

private:

	bool parse_name(const StaticString &name);

	bool parse_value(const StaticString &value);

	bool parse_lbrace();

	bool parse_rbrace();

	bool parse_lbracket();

	bool parse_rbracket();

	bool parse_end();

private:

	Document &_document;

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
//		LParen,   // '('.
//		RParen,   // ')'.
		Comment,  // '#'.
	};

	// NOTE: LParen and RParen are reserved for possible future parenthesized string notation.

	static const CharClass char_class[256];

private:

	struct State
	{
		Object *object;
		List   *list;

		State(Object *object)
			: object(object)
			, list(nullptr)
		{
		}

		State(List *list)
			: object(nullptr)
			, list(list)
		{
		}
	};

	typedef std::vector<State> States;

	States  _states;
	State  *_state;
};

} // namespace Ion

} // namespace Yttrium

#endif // __ION_PARSER_H
