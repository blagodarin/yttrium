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
				End,
				Identifier,
				Number,
				String,
				Separator, // Command separator (newline or semicolon).
				Equals,
			};

			size_t line;
			std::ptrdiff_t column;
			Type type;
			StaticString string;
		};

		explicit ScriptScanner(std::string& text);

		Token read();

	private:
		char* _cursor;
		const char* const _end;
		const char* _line_origin = _cursor - 1;
		size_t _line = 1;
	};
}

#endif
