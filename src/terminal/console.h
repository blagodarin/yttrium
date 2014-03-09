#ifndef __TERMINAL_CONSOLE_H
#define __TERMINAL_CONSOLE_H

#include <yttrium/gui/logic/line_editor.h>

namespace Yttrium
{

class RendererBuiltin;
class Terminal;

class Console
{
public:

	Console(const Terminal &terminal, Allocator *allocator)
		: _terminal(terminal)
		, _line_editor(allocator)
	{
	}

public:

	bool process_key(Key key);
	void render_input(RendererBuiltin *renderer, int x, int y, int max_size);

private:

	const Terminal &_terminal;
	LineEditor      _line_editor;
};

} // namespace Yttrium

#endif // __TERMINAL_CONSOLE_H
