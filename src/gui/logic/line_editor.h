#ifndef _src_gui_logic_line_editor_h_
#define _src_gui_logic_line_editor_h_

#include <yttrium/std/string_view.h>

namespace Yttrium
{
	class KeyEvent;

	class LineEditor
	{
	public:
		void clear();
		size_t cursor() const { return _cursor; }
		void insert(std::string_view);
		void insert(char symbol) { insert({&symbol, 1}); }
		bool process_key(const KeyEvent&);
		void reset(std::string&&);
		size_t selection_offset() const { return _selection_offset; }
		size_t selection_size() const { return _selection_size; }
		void set_max_size(size_t);
		const std::string& text() const { return _text; }

	private:
		std::string _text;
		size_t _cursor = 0;
		size_t _selection_size = 0;
		size_t _selection_offset = 0;
		size_t _max_size = _text.max_size();
	};
}

#endif
