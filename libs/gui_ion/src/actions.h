// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/script/code.h>

#include <variant>
#include <vector>

namespace Yt
{
	class IonGuiPrivate;

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

		void run(IonGuiPrivate&) const;

	private:
		std::vector<std::variant<GuiAction_Quit, GuiAction_Call, GuiAction_Enter, GuiAction_Return, GuiAction_ReturnTo>> _actions;
	};
}
