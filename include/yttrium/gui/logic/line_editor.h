/// \file
/// \brief

#ifndef __Y_GUI_LOGIC_LINE_EDITOR_H
#define __Y_GUI_LOGIC_LINE_EDITOR_H

#include <yttrium/key.h>
#include <yttrium/string.h>

namespace Yttrium
{

/// Command line console.

class Y_API LineEditor
{
public:

	///

	LineEditor(Allocator *allocator = DefaultAllocator) noexcept;

public:

	///

	void clear() noexcept;

	///

	inline size_t cursor() noexcept;

	///

	void insert(const StaticString &text) noexcept;

	/**
	* \overload
	* \param symbol The symbol to insert.
	*/

	inline void insert(char symbol) noexcept;

	/// Process a \a key with the specified \a shift state.
	/// \param key Input key.
	/// \param shift Should be \c true if a shift key is pressed.
	/// \return \c true if the key was handled.
	/// \note This function handles control keys only.

	bool process_key(Key key, bool shift = false) noexcept;

	/// Return the selection offset.
	/// \return Current selection offset.
	/// \note This function may return anything if the selection size is zero.

	inline size_t selection_offset() noexcept;

	///

	inline size_t selection_size() noexcept;

	///

	inline String text() noexcept;

private:

	String _text;
	size_t _cursor;
	size_t _selection_size;
	size_t _selection_offset;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t LineEditor::cursor() noexcept
{
	return _cursor;
}

void LineEditor::insert(char symbol) noexcept
{
	insert(StaticString(&symbol, 1));
}

size_t LineEditor::selection_offset() noexcept
{
	return _selection_offset;
}

size_t LineEditor::selection_size() noexcept
{
	return _selection_size;
}

String LineEditor::text() noexcept
{
	return _text;
}

} // namespace Yttrium

#endif // __Y_GUI_LOGIC_LINE_EDITOR_H
