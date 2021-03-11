// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/gui.h>

#include <yttrium/application/key.h>
#include <yttrium/application/window.h>
#include <yttrium/math/point.h>
#include <yttrium/math/rect.h>

#include <algorithm>
#include <vector>

namespace Yt
{
	class GuiStateData
	{
	public:
		static constexpr uint16_t kPayloadMask = 0x00ff;
		static constexpr uint16_t kPressedFlag = 0x0100;
		static constexpr uint16_t kAutorepeatFlag = 0x0200;
		static constexpr uint16_t kTextFlag = 0x4000;
		static constexpr uint16_t kProcessedFlag = 0x8000;
		static constexpr uint16_t kKeySearchMask = kPayloadMask | kTextFlag | kProcessedFlag;

		Window& _window;
		std::optional<Vector2> _cursor;
		std::vector<uint16_t> _inputEvents;

		GuiStateData(Window& window) noexcept
			: _window{ window } {}
	};

	GuiState::GuiState(Window& window)
		: _data{ std::make_unique<GuiStateData>(window) }
	{
	}

	GuiState::~GuiState() noexcept = default;

	void GuiState::processKeyEvent(const KeyEvent& event)
	{
		auto encodedEvent = static_cast<uint16_t>(event._key);
		if (event._pressed)
		{
			encodedEvent |= GuiStateData::kPressedFlag;
			if (event._autorepeat)
				encodedEvent |= GuiStateData::kAutorepeatFlag;
		}
		_data->_inputEvents.emplace_back(encodedEvent);
	}

	GuiFrame::GuiFrame(GuiState& state)
		: _state{ *state._data }
	{
		_state._cursor.emplace(_state._window.cursor());
	}

	GuiFrame::~GuiFrame() noexcept
	{
		_state._inputEvents.clear();
	}

	std::optional<Vector2> GuiFrame::captureCursor(const RectF& rect) noexcept
	{
		std::optional<Vector2> captured;
		if (_state._cursor && rect.contains(*_state._cursor))
			captured.swap(_state._cursor);
		return captured;
	}

	bool GuiFrame::captureKeyDown(Key key, bool autorepeat) noexcept
	{
		bool captured = false;
		const auto i = std::find_if(_state._inputEvents.begin(), _state._inputEvents.end(), [key](const auto event) {
			return (event & GuiStateData::kKeySearchMask) == static_cast<uint8_t>(key);
		});
		if (i != _state._inputEvents.end())
		{
			*i |= GuiStateData::kProcessedFlag;
			if ((*i & (GuiStateData::kPressedFlag | (autorepeat ? 0 : GuiStateData::kAutorepeatFlag))) == GuiStateData::kPressedFlag)
				captured = true;
			for (auto j = std::next(i); j != _state._inputEvents.end(); ++j)
				if ((*j & GuiStateData::kKeySearchMask) == static_cast<uint8_t>(key))
					*j = GuiStateData::kProcessedFlag;
		}
		return captured;
	}

	Vector2 GuiFrame::cursor() const noexcept
	{
		return Vector2{ _state._window.cursor() };
	}
}
