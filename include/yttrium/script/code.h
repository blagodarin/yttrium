/// \file
/// \brief Parsed script code.

#ifndef _include_yttrium_script_code_h_
#define _include_yttrium_script_code_h_

#include <yttrium/base.h>
#include <yttrium/memory/global.h>

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

		ScriptCode() = default;

		///
		explicit ScriptCode(String&& text, Allocator* allocator = DefaultAllocator);

		///
		explicit ScriptCode(const StaticString& text, Allocator* allocator = DefaultAllocator);

		/// Execute the script.
		/// \param context Context to execute the script in.
		/// \param mode Execution mode.
		void execute(ScriptContext& context, ExecutionMode mode = Do) const;

		///
		static ScriptCode load(const StaticString& filename, Allocator* allocator = DefaultAllocator);

	private:
		Y_UNIQUE_PRIVATE(ScriptCode);
	};
}

#endif
