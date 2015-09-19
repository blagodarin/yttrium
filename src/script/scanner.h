#ifndef _src_script_scanner_h_
#define _src_script_scanner_h_

#include <yttrium/static_string.h>

namespace Yttrium
{
	class StaticString;

	class ScriptScanner
	{
	public:

		struct Token
		{
			enum Type
			{
				Identifier,
				XIdentifier, // "+identifier" and "-identifier".
				Literal,
				String,
				Separator, // Command separator (newline or semicolon).
				End,
			};

			size_t       line;
			size_t       column;
			Type         type;
			StaticString string;
		};

	public:

		ScriptScanner(const StaticString& text);

		bool read(Token& token);

	private:

		char*       _cursor;
		const char* _end;
		char*       _line_origin;
		size_t      _line;
	};
}

#endif
