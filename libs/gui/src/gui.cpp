// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/gui.h>

#include <yttrium/application/key.h>
#include <yttrium/application/window.h>
#include <yttrium/math/point.h>
#include <yttrium/math/rect.h>

#include <algorithm>
#include <cassert>
#include <string>
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
		std::string _cursorItem;
		Key _cursorItemKey = Key::Null;

		explicit GuiStateData(Window& window) noexcept
			: _window{ window } {}

		std::pair<unsigned, bool> captureClick(Key key, bool autorepeat) noexcept
		{
			const auto i = std::find_if(_inputEvents.begin(), _inputEvents.end(), [key](const auto event) {
				return (event & kKeySearchMask) == static_cast<uint8_t>(key);
			});
			if (i == _inputEvents.end())
				return { 0u, false };
			*i |= kProcessedFlag;
			if (!(*i & kPressedFlag))
				return { 0u, true };
			auto count = static_cast<unsigned>(!(*i & kAutorepeatFlag) || autorepeat);
			for (auto j = std::next(i); j != _inputEvents.end(); ++j)
			{
				if ((*j & kKeySearchMask) != static_cast<uint8_t>(key))
					continue;
				*j |= kProcessedFlag;
				if (!(*j & kPressedFlag))
					return { count, true };
				assert(*j & kAutorepeatFlag); // Either it is autorepeat, or we've missed some events.
				if (autorepeat)
					++count;
			}
			return { count, false };
		}
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
		else if (!_data->_cursorItem.empty() && _data->_cursorItemKey == event._key)
		{
			_data->_cursorItem.clear();
			_data->_cursorItemKey = Key::Null;
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

	bool GuiFrame::captureKeyDown(Key key) noexcept
	{
		return _state.captureClick(key, false).first > 0;
	}

	std::optional<Vector2> GuiFrame::dragArea(std::string_view id, const RectF& rect, Key key)
	{
		assert(!id.empty());
		if (_state._cursorItem == id)
		{
			assert(_state._cursor);
			auto captured = rect.bound(*_state._cursor);
			_state._cursor.reset();
			return captured;
		}
		if (_state._cursorItem.empty())
			if (auto maybeCaptured = hoverArea(rect))
				if (const auto [down, up] = _state.captureClick(key, false); down > 0)
				{
					if (!up)
					{
						_state._cursorItem = id; // May throw std::bad_alloc.
						_state._cursorItemKey = key;
					}
					return maybeCaptured;
				}
		return {};
	}

	std::optional<Vector2> GuiFrame::hoverArea(const RectF& rect) noexcept
	{
		std::optional<Vector2> captured;
		if (_state._cursor && rect.contains(*_state._cursor))
			captured.swap(_state._cursor);
		return captured;
	}
}
