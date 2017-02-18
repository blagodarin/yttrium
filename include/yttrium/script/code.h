/// \file
/// \brief Parsed script code.

#ifndef _include_yttrium_script_code_h_
#define _include_yttrium_script_code_h_

#include <yttrium/memory/global.h>

#include <memory>

namespace Yttrium
{
	class ScriptContext;
	class StaticString;

	/// Script code.
	class Y_API ScriptCode
	{
	public:
		///
		ScriptCode();

		///
		explicit ScriptCode(std::string&&, Allocator& = *DefaultAllocator);

		///
		explicit ScriptCode(const StaticString&, Allocator& = *DefaultAllocator);

		///
		explicit operator bool() const noexcept { return static_cast<bool>(_private); }

		/// Executes the script.
		void execute(ScriptContext&) const;

		///
		static ScriptCode load(const StaticString& filename, Allocator& = *DefaultAllocator);

	private:
		std::unique_ptr<class ScriptCodePrivate> _private;

	public:
		~ScriptCode();
		ScriptCode(ScriptCode&&) noexcept;
		ScriptCode& operator=(ScriptCode&&) noexcept;
	};
}

#endif
