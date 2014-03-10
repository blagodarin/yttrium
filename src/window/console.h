#ifndef __WINDOW_CONSOLE_H
#define __WINDOW_CONSOLE_H

#include <yttrium/gui/logic/line_editor.h>

namespace Yttrium
{

class RendererBuiltin;
class Window;

class Console
{
public:

	Console(const Window& window, Allocator* allocator)
		: _window(window)
		, _line_editor(allocator)
	{
	}

public:

	bool process_key(Key key);
	void render_input(RendererBuiltin* renderer, int x, int y, int max_size);

private:

	const Window& _window;
	LineEditor    _line_editor;
};

} // namespace Yttrium

#endif // __WINDOW_CONSOLE_H
