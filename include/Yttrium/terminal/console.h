/// \file
/// \brief Command line console.

#ifndef __Y_TERMINAL_CONSOLE_H
#define __Y_TERMINAL_CONSOLE_H

#include <Yttrium/key.h>
#include <Yttrium/string.h>

namespace Yttrium
{

class RendererBuiltin;
class Terminal;

/// Command line console.

class Y_API Console
{
public:

	///

	Console(const Terminal &terminal) noexcept;

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

	String _input;
	size_t _cursor;
	size_t _selection_size;
	size_t _selection_offset;
};

////////////////////////////////////////////////////////////////////////////////

inline Console::Console(const Terminal &terminal) noexcept
	: _terminal(terminal)
	, _cursor(0)
	, _selection_size(0)
	, _selection_offset(0)
{
}

} // namespace Yttrium

#endif // __Y_TERMINAL_CONSOLE_H
