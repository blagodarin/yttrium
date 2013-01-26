/// \file
/// \brief Command line console.

#ifndef __Y_TERMINAL_CONSOLE_H
#define __Y_TERMINAL_CONSOLE_H

#include <Yttrium/gui/logic/line_editor.h>

namespace Yttrium
{

class RendererBuiltin;
class Terminal;

/// Command line console.

class Y_API Console
{
public:

	~Console() = default;

	///

	inline Console(const Terminal &terminal, Allocator *allocator = DefaultAllocator) noexcept;

public:

	/// Process a \a key from the specified \a terminal.
	/// \return \c true if the key was handled.

	bool process_key(Key key) noexcept;

	/// Render the input line in the specified position.
	/// \param renderer RendererBuiltin to render with.
	/// \param x Left offset.
	/// \param y Top offset.

	void render_input(RendererBuiltin *renderer, Dim x, Dim y, Dim max_size = -1) noexcept;

private:

	const Terminal &_terminal;
	LineEditor      _line_editor;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Console::Console(const Terminal &terminal, Allocator *allocator) noexcept
	: _terminal(terminal)
	, _line_editor(allocator)
{
}

} // namespace Yttrium

#endif // __Y_TERMINAL_CONSOLE_H
