// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "context_data.h"

#include <yttrium/gui/layout.h>

#include <cassert>

namespace
{
	Yt::GuiLayout kNullLayout;
}

namespace Yt
{
	GuiContextData::GuiContextData(Window& window) noexcept
		: _window{ window }
		, _layout{ &kNullLayout }
	{
	}

	void GuiContextData::onWindowMouseMove([[maybe_unused]] const Window& window, int, int)
	{
		assert(&window == &_window);
	}

	void GuiContextData::onWindowKeyEvent([[maybe_unused]] const Window& window, const KeyEvent& event)
	{
		assert(&window == &_window);
		auto encodedEvent = static_cast<uint16_t>(event._key);
		if (event._pressed)
		{
			encodedEvent |= kPressedFlag;
			if (event._autorepeat)
				encodedEvent |= kAutorepeatFlag;
			if (event._modifiers & KeyEvent::Modifier::Shift)
				encodedEvent |= kShiftFlag;
		}
		_inputEvents.emplace_back(encodedEvent);
		_keyStates[static_cast<uint8_t>(event._key)] = event._pressed ? kKeyStatePressed : 0;
	}

	void GuiContextData::onWindowTextInput([[maybe_unused]] const Window& window, std::string_view text)
	{
		assert(&window == &_window);
		const auto index = _textInputs.size();
		if (index >= kPayloadMask)
			return;
		_inputEvents.reserve(_inputEvents.size() + 1);
		_textInputs.emplace_back(text);
		_inputEvents.emplace_back(static_cast<uint16_t>(kTextFlag | index));
	}
}
