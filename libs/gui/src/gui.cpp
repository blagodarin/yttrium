// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/gui.h>

#include <yttrium/application/key.h>
#include <yttrium/application/window.h>
#include <yttrium/gui/font.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/2d.h>

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
		GuiButtonStyle _buttonStyle;
		std::shared_ptr<const Font> _defaultFont;

		explicit GuiStateData(Window& window) noexcept
			: _window{ window } {}

		std::pair<unsigned, bool> captureClick(Key key, bool autorepeat, bool release = false) noexcept
		{
			const auto i = std::find_if(_inputEvents.begin(), _inputEvents.end(), [key](const auto event) {
				return (event & kKeySearchMask) == static_cast<uint8_t>(key);
			});
			if (i == _inputEvents.end())
				return { 0u, false };
			if (release && (*i & kPressedFlag))
				return { 0u, true };
			*i |= kProcessedFlag;
			if (!(*i & kPressedFlag))
				return { 0u, true };
			auto count = static_cast<unsigned>(!(*i & kAutorepeatFlag) || autorepeat);
			for (auto j = std::next(i); j != _inputEvents.end(); ++j)
			{
				if ((*j & kKeySearchMask) == static_cast<uint8_t>(key))
				{
					if (!(*j & kAutorepeatFlag))
					{
						if (!(*j & kPressedFlag))
							*j |= kProcessedFlag;
						return { count, true };
					}
					assert(*j & kPressedFlag);
					*j |= kProcessedFlag;
					if (autorepeat)
						++count;
				}
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
		_data->_inputEvents.emplace_back(encodedEvent);
	}

	void GuiState::setDefaultFont(const std::shared_ptr<const Font>& font) noexcept
	{
		_data->_defaultFont = font;
	}

	GuiFrame::GuiFrame(GuiState& state, Renderer2D& renderer)
		: _state{ *state._data }
		, _renderer{ renderer }
	{
		_state._cursor.emplace(_state._window.cursor());
		setButtonStyle({});
	}

	GuiFrame::~GuiFrame() noexcept
	{
		if (_state._cursorItemKey != Key::Null && _state.captureClick(_state._cursorItemKey, false, true).second)
		{
			_state._cursorItem.clear();
			_state._cursorItemKey = Key::Null;
		}
		_state._inputEvents.clear();
	}

	bool GuiFrame::button(std::string_view id, std::string_view text, const RectF& rect)
	{
		assert(!id.empty());
		bool clicked = false;
		const auto* styleState = &_state._buttonStyle._normal;
		if (_state._cursorItem == id)
		{
			assert(_state._cursor);
			const auto hovered = rect.contains(*_state._cursor);
			const auto released = _state.captureClick(_state._cursorItemKey, false, true).second;
			if (released)
			{
				_state._cursorItem.clear();
				_state._cursorItemKey = Key::Null;
				if (hovered)
				{
					clicked = true;
					styleState = &_state._buttonStyle._hovered;
					_state._cursor.reset();
				}
			}
			else
			{
				styleState = &_state._buttonStyle._pressed;
				_state._cursor.reset();
			}
		}
		else if (_state._cursorItem.empty() && hoverArea(rect))
		{
			styleState = &_state._buttonStyle._hovered;
			if (const auto [pressed, released] = _state.captureClick(Key::Mouse1, false); pressed)
			{
				if (!released)
				{
					_state._cursorItem = id; // May throw std::bad_alloc.
					_state._cursorItemKey = Key::Mouse1;
					styleState = &_state._buttonStyle._pressed;
				}
				else
					clicked = true;
			}
		}
		_renderer.setTexture({});
		_renderer.setColor(styleState->_backgroundColor);
		_renderer.addRect(rect);
		if (_state._buttonStyle._font)
		{
			const auto fontSize = rect.height() * _state._buttonStyle._fontSize;
			const auto textSize = _state._buttonStyle._font->text_size(text, { 1, fontSize });
			_renderer.setColor(styleState->_textColor);
			_state._buttonStyle._font->render(_renderer, rect.top_left() + Vector2{ rect.width() - textSize._width, rect.height() - textSize._height } / 2, fontSize, text);
		}
		return clicked;
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
				if (const auto [pressed, released] = _state.captureClick(key, false); pressed)
				{
					if (!released)
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

	void GuiFrame::setButtonStyle(const GuiButtonStyle& style) noexcept
	{
		_state._buttonStyle = style;
		if (!_state._buttonStyle._font)
			_state._buttonStyle._font = _state._defaultFont;
	}
}
