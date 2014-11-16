/// \file
/// \brief Parsed script code.

#ifndef __Y_SCRIPT_CODE_H
#define __Y_SCRIPT_CODE_H

#include <yttrium/types.h> // ExecutionMode

namespace Yttrium
{

class ScriptContext;
class StaticString;
class String;

/// Script code.
class Y_API ScriptCode
{
	Y_DECLARE_PRIVATE(ScriptCode);

public:

	///
	explicit ScriptCode(String&& text) noexcept;

	///
	explicit ScriptCode(const StaticString& text) noexcept;

	/// Execute the script.
	/// \param context Context to execute the script in, or \c nullptr for the global context.
	/// \param mode Execution mode.
	void execute(ScriptContext* context = nullptr, ExecutionMode mode = ExecutionMode::Do) const noexcept;

	///
	static ScriptCode load(const StaticString& filename) noexcept;
};

} // namespace Yttrium

#endif // __Y_SCRIPT_CODE_H
