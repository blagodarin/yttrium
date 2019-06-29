//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "actions.h"

#include "gui.h"

namespace
{
	using namespace Yttrium;

	// TODO: Handle action execution errors.
	class GuiActionVisitor
	{
	public:
		explicit GuiActionVisitor(GuiPrivate& gui)
			: _gui{ gui } {}

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
			std::visit(visitor, action);
	}
}
