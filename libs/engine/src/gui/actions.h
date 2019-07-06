//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#pragma once

#include <yttrium/script/code.h>

#include <variant>
#include <vector>

namespace Yttrium
{
	class GuiPrivate;

	struct GuiAction_Call
	{
		ScriptCode _script;
		explicit GuiAction_Call(std::string_view script)
			: _script{ std::string{ script } } {}
	};

	struct GuiAction_Enter
	{
		std::string _screen;
		explicit GuiAction_Enter(std::string_view screen)
			: _screen{ screen } {}
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
		explicit GuiAction_ReturnTo(std::string_view screen)
			: _screen{ screen } {}
	};

	class GuiActions
	{
	public:
		template <typename T, typename... Args>
		void add(Args&&... args)
		{
			_actions.emplace_back(T(std::forward<Args>(args)...));
		}

		void run(GuiPrivate&) const;

	private:
		std::vector<std::variant<GuiAction_Quit, GuiAction_Call, GuiAction_Enter, GuiAction_Return, GuiAction_ReturnTo>> _actions;
	};
}
