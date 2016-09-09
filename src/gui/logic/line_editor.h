#ifndef _src_gui_logic_line_editor_h_
#define _src_gui_logic_line_editor_h_

#include <yttrium/string.h>

namespace Yttrium
{
	class KeyEvent;

	class LineEditor
	{
	public:
		LineEditor(Allocator& allocator);

		void clear();
		unsigned cursor() const { return _cursor; }
		void insert(const StaticString& text);
		void insert(char symbol) { insert(StaticString(&symbol, 1)); }
		bool process_key(const KeyEvent& event);
		unsigned selection_offset() const { return _selection_offset; }
		unsigned selection_size() const { return _selection_size; }
		void set_max_size(unsigned max_size);
		const String& text() const { return _text; }

	private:
		String _text;
		unsigned _cursor = 0;
		unsigned _selection_size = 0;
		unsigned _selection_offset = 0;
		unsigned _max_size = 0;
	};
}

#endif
