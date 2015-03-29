#ifndef __GUI_LOGIC_LINE_EDITOR_H
#define __GUI_LOGIC_LINE_EDITOR_H

#include <yttrium/string.h>

namespace Yttrium
{
	class KeyEvent;

	class LineEditor
	{
	public:

		LineEditor(Allocator* allocator);

		void clear();
		unsigned cursor() const { return _cursor; }
		void insert(const StaticString& text);
		void insert(char symbol) { insert(StaticString(&symbol, 1)); }
		bool process_key(const KeyEvent& event);
		unsigned selection_offset() const { return _selection_offset; }
		unsigned selection_size() const { return _selection_size; }
		void set_max_size(unsigned max_size);
		String text() const { return _text; }

	private:

		String   _text;
		unsigned _cursor;
		unsigned _selection_size;
		unsigned _selection_offset;
		unsigned _max_size;
	};
}

#endif // __GUI_LOGIC_LINE_EDITOR_H
