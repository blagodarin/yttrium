#ifndef __SCRIPT_SCANNER_H
#define __SCRIPT_SCANNER_H

#include <yttrium/static_string.h>

namespace Yttrium
{

class ScriptScanner
{
public:

	class Token
	{
	public:

		enum Type
		{
			Identifier,
			XIdentifier, // "+identifier" and "-identifier".
			Literal,
			String,
			Separator, // Command separator (newline or semicolon).
			End,
		};

	public:

		size_t       line;
		size_t       column;
		Type         type;
		StaticString string;
	};

public:

	ScriptScanner(const StaticString &text);

	bool read(Token *token);

private:

	char       *_cursor;
	const char *_end;
	char       *_line_origin;
	size_t      _line;
};

} // namespace Yttrium

#endif // __SCRIPT_SCANNER_H
