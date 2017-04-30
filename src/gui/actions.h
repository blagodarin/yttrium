#ifndef _src_gui_actions_h_
#define _src_gui_actions_h_

#include <yttrium/script/code.h>
#include <yttrium/std_variant.h>

#include <vector>

namespace Yttrium
{
	class GuiPrivate;

	struct GuiAction_Call
	{
		ScriptCode _script;
		explicit GuiAction_Call(std::string&& script) : _script(std::move(script)) {}
	};

	struct GuiAction_Enter
	{
		std::string _screen;
		explicit GuiAction_Enter(const std::string& screen) : _screen(screen) {}
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
		explicit GuiAction_ReturnTo(const std::string& screen) : _screen(screen) {}
	};

	class GuiActions
	{
	public:
		GuiActions() = default;
		explicit GuiActions(size_t reserve) { _actions.reserve(reserve); }

		template <typename T, typename... Args>
		void add(Args&&... args) { _actions.emplace_back(T(std::forward<Args>(args)...)); }

		void run(GuiPrivate&) const;

	private:
		std::vector<std::variant<GuiAction_Quit, GuiAction_Call, GuiAction_Enter, GuiAction_Return, GuiAction_ReturnTo>> _actions;
	};
}

#endif
