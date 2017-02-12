#ifndef _src_gui_actions_h_
#define _src_gui_actions_h_

#include <yttrium/script/code.h>

#include <vector>

#include <boost/variant/variant.hpp> // TODO: Use std::variant in C++17.

namespace Yttrium
{
	class GuiPrivate;

	struct GuiAction_Call
	{
		ScriptCode _script;
		GuiAction_Call(const StaticString& script) : _script(script) {}
	};

	struct GuiAction_Enter
	{
		std::string _screen;
		GuiAction_Enter(const std::string& screen) : _screen(screen) {}
	};

	struct GuiAction_Quit
	{
	};

	struct GuiAction_Return
	{
	};

	struct GuiAction_ReturnTo
	{
		std::string _screen;
		GuiAction_ReturnTo(const std::string& screen) : _screen(screen) {}
	};

	class GuiActions
	{
	public:
		GuiActions(size_t reserve = 0) { _actions.reserve(reserve); }

		template <typename T, typename... Args>
		void add(Args&&... args) { _actions.emplace_back(T(std::forward<Args>(args)...)); }

		void run(GuiPrivate&) const;

	private:
		std::vector<boost::variant<GuiAction_Quit, GuiAction_Call, GuiAction_Enter, GuiAction_Return, GuiAction_ReturnTo>> _actions;
	};
}

#endif
