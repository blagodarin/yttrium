//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef _src_script_scanner_h_
#define _src_script_scanner_h_

#include <string_view>

namespace Yttrium
{
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
			std::string_view string;
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
