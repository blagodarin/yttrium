#include "actions.h"

#include "gui.h"

#include <boost/variant/apply_visitor.hpp>

namespace
{
	using namespace Yttrium;

	// TODO: Handle action execution errors.
	class GuiActionVisitor
	{
	public:
		explicit GuiActionVisitor(GuiPrivate& gui) : _gui(gui) {}

		void operator()(const GuiAction_Call& action) const { action._script.execute(_gui.script_context()); }
		void operator()(const GuiAction_Enter& action) const { _gui.push_screen(action._screen); }
		void operator()(const GuiAction_Quit&) const { _gui.quit(); }
		void operator()(const GuiAction_Return&) const { _gui.pop_screen(); }
		void operator()(const GuiAction_ReturnTo& action) const { _gui.pop_screens_until(action._screen); }

	private:
		GuiPrivate& _gui;
	};
}

namespace Yttrium
{
	void GuiActions::run(GuiPrivate& gui) const
	{
		const GuiActionVisitor visitor(gui);
		for (const auto& action : _actions)
			boost::apply_visitor(visitor, action);
	}
}
