#ifndef _include_yttrium_script_code_h_
#define _include_yttrium_script_code_h_

#include <yttrium/api.h>

#include <memory>
#include <string>

namespace Yttrium
{
	class ScriptContext;

	/// Parsed script code.
	class Y_API ScriptCode
	{
	public:
		///
		ScriptCode();

		///
		~ScriptCode();

		///
		explicit ScriptCode(std::string&&);

		///
		explicit operator bool() const noexcept { return static_cast<bool>(_private); }

		/// Executes the script.
		void execute(ScriptContext&) const;

		ScriptCode(ScriptCode&&) noexcept;
		ScriptCode& operator=(ScriptCode&&) noexcept;

	private:
		std::unique_ptr<class ScriptCodePrivate> _private;
	};
}

#endif
