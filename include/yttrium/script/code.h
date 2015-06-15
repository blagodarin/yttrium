/// \file
/// \brief Parsed script code.

#ifndef __Y_SCRIPT_CODE_H
#define __Y_SCRIPT_CODE_H

#include <yttrium/base.h>
#include <yttrium/global.h>

namespace Yttrium
{
	class ScriptContext;
	class StaticString;
	class String;

	/// Script code.
	class Y_API ScriptCode
	{
	public:

		///
		enum ExecutionMode
		{
			Do,   ///<
			Undo, ///<
		};

		///
		explicit ScriptCode(String&& text);

		///
		explicit ScriptCode(const StaticString& text);

		/// Execute the script.
		/// \param context Context to execute the script in, or \c nullptr for the global context.
		/// \param mode Execution mode.
		void execute(ScriptContext* context = nullptr, ExecutionMode mode = Do) const;

		///
		static ScriptCode load(const StaticString& filename);

	private:
		Y_SHARED_PRIVATE(ScriptCode);
	};
}

#endif // __Y_SCRIPT_CODE_H
