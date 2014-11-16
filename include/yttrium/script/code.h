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
public:

	ScriptCode() noexcept;
	ScriptCode(const ScriptCode&) noexcept;
	ScriptCode(ScriptCode&&) noexcept;
	~ScriptCode() noexcept;

	ScriptCode& operator=(const ScriptCode&) noexcept;
	ScriptCode& operator=(ScriptCode&&) noexcept;

	///
	explicit ScriptCode(String&& text) noexcept;

	///
	explicit ScriptCode(const StaticString& text) noexcept;

	///
	explicit operator bool() const noexcept { return _private; }

	/// Execute the script.
	/// \param context Context to execute the script in, or \c nullptr for the global context.
	/// \param mode Execution mode.
	void execute(ScriptContext* context = nullptr, ExecutionMode mode = ExecutionMode::Do) const noexcept;

	///
	static ScriptCode load(const StaticString& filename) noexcept;

private:

	class Private;

	Private* _private;

	explicit ScriptCode(Private* private_): _private(private_) {}
};

} // namespace Yttrium

#endif // __Y_SCRIPT_CODE_H
