/// \file
/// \brief Parsed script code.

#ifndef _include_yttrium_script_code_h_
#define _include_yttrium_script_code_h_

#include <yttrium/memory/unique_ptr.h>

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
		ScriptCode();

		///
		explicit ScriptCode(String&& text, Allocator* allocator = DefaultAllocator);

		///
		explicit ScriptCode(const StaticString& text, Allocator* allocator = DefaultAllocator);

		///
		explicit operator bool() const noexcept { return static_cast<bool>(_private); }

		/// Execute the script.
		/// \param context Context to execute the script in.
		/// \param mode Execution mode.
		void execute(ScriptContext& context, ExecutionMode mode = Do) const;

		///
		static ScriptCode load(const StaticString& filename, Allocator* allocator = DefaultAllocator);

	private:
		class Private;
		UniquePtr<Private> _private;
	public:
		ScriptCode(ScriptCode&&) = default;
		~ScriptCode();
		ScriptCode& operator=(ScriptCode&&);
	};
}

#endif
