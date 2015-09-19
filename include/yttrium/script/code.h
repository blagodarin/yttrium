/// \file
/// \brief Parsed script code.

#ifndef _include_yttrium_script_code_h_
#define _include_yttrium_script_code_h_

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

#endif
