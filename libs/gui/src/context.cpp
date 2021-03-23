// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/context.h>

#include "context_data.h"

namespace Yt
{
	GuiContext::GuiContext(Window& window)
		: _data{ std::make_unique<GuiContextData>(window) }
	{
	}

	GuiContext::~GuiContext() noexcept = default;

	void GuiContext::processKeyEvent(const KeyEvent& event)
	{
		auto encodedEvent = static_cast<uint16_t>(event._key);
		if (event._pressed)
		{
			encodedEvent |= GuiContextData::kPressedFlag;
			if (event._autorepeat)
				encodedEvent |= GuiContextData::kAutorepeatFlag;
			if (event._modifiers & KeyEvent::Modifier::Shift)
				encodedEvent |= GuiContextData::kShiftFlag;
		}
		_data->_inputEvents.emplace_back(encodedEvent);
		_data->_keyStates[static_cast<uint8_t>(event._key)] = event._pressed ? GuiContextData::kKeyStatePressed : 0;
	}

	void GuiContext::processTextInput(std::string_view text)
	{
		const auto index = _data->_textInputs.size();
		if (index >= GuiContextData::kPayloadMask)
			return;
		_data->_inputEvents.reserve(_data->_inputEvents.size() + 1);
		_data->_textInputs.emplace_back(text);
		_data->_inputEvents.emplace_back(static_cast<uint16_t>(GuiContextData::kTextFlag | index));
	}

	void GuiContext::setDefaultFont(const std::shared_ptr<const Font>& font) noexcept
	{
		_data->_defaultFont = font;
	}
}
