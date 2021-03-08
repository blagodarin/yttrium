// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "actions.h"

#include "gui.h"

namespace
{
	// TODO: Handle action execution errors.
	class GuiActionVisitor
	{
	public:
		explicit GuiActionVisitor(Yt::IonGuiPrivate& gui)
			: _gui{ gui } {}

		void operator()(const Yt::GuiAction_Call& action) const { action._script.execute(_gui.script_context()); }
		void operator()(const Yt::GuiAction_Enter& action) const { _gui.push_screen(action._screen); }
		void operator()(const Yt::GuiAction_Quit&) const { _gui.quit(); }
		void operator()(const Yt::GuiAction_Return&) const { _gui.pop_screen(); }
		void operator()(const Yt::GuiAction_ReturnTo& action) const { _gui.pop_screens_until(action._screen); }

	private:
		Yt::IonGuiPrivate& _gui;
	};
}

namespace Yt
{
	void GuiActions::run(IonGuiPrivate& gui) const
	{
		const GuiActionVisitor visitor(gui);
		for (const auto& action : _actions)
			std::visit(visitor, action);
	}
}
