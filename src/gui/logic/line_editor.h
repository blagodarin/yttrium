#ifndef __GUI_LOGIC_LINE_EDITOR_H
#define __GUI_LOGIC_LINE_EDITOR_H

#include <yttrium/key.h>
#include <yttrium/string.h>

namespace Yttrium
{

class LineEditor
{
public:

	LineEditor(Allocator* allocator);

public:

	void clear();

	size_t cursor()
	{
		return _cursor;
	}

	void insert(const StaticString& text);

	void insert(char symbol)
	{
		insert(StaticString(&symbol, 1));
	}

	bool process_key(Key key, bool shift);

	size_t selection_offset()
	{
		return _selection_offset;
	}

	size_t selection_size()
	{
		return _selection_size;
	}

	String text()
	{
		return _text;
	}

private:

	String _text;
	size_t _cursor;
	size_t _selection_size;
	size_t _selection_offset;
};

} // namespace Yttrium

#endif // __GUI_LOGIC_LINE_EDITOR_H
