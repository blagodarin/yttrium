// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/gui.h>

#include <yttrium/application/key.h>
#include <yttrium/application/window.h>
#include <yttrium/gui/font.h>
#include <yttrium/gui/layout.h>
#include <yttrium/gui/style.h>
#include <yttrium/gui/text_capture.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/2d.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <string>
#include <vector>

#include <primal/utf8.hpp>

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
		std::vector<uint16_t> _inputEvents;
		std::optional<Vector2> _mouseCursor;
		std::string _mouseItem;
		bool _mouseItemPresent = false;
		Key _mouseItemKey = Key::Null;
		std::string _keyboardItem;
		bool _keyboardItemPresent = false;
		size_t _keyboardItemCursor = 0;
		std::chrono::steady_clock::time_point _keyboardItemCursorMark;
		GuiButtonStyle _buttonStyle;
		GuiEditStyle _editStyle;
		GuiLabelStyle _labelStyle;
		std::shared_ptr<const Font> _defaultFont;
		std::shared_ptr<const Texture2D> _blankTexture;
		RectF _blankTextureRect;
		GuiLayout _layout{ {} };

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

		void captureKeyboard(std::function<bool(Key)>&& callback)
		{
			assert(!_keyboardItem.empty());
			bool processed = false;
			for (auto& event : _inputEvents)
			{
				if (!(event & (kTextFlag | kProcessedFlag)))
				{
					event |= kProcessedFlag;
					processed = true;
					if ((event & kPressedFlag) && !callback(static_cast<Key>(event & kPayloadMask)))
						break;
				}
			}
			if (processed)
				_keyboardItemCursorMark = std::chrono::steady_clock::now();
		}

		std::optional<Vector2> captureMouse(const RectF& rect) noexcept
		{
			std::optional<Vector2> captured;
			if (_mouseCursor && rect.contains(*_mouseCursor))
				captured.swap(_mouseCursor);
			return captured;
		}

		void updateBlankTexture(const std::shared_ptr<const Font>& font) noexcept
		{
			if (font)
			{
				_blankTexture = font->texture();
				_blankTextureRect = font->white_rect();
			}
			else
				_blankTexture = {};
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
		_state._mouseCursor.emplace(_state._window.cursor());
		_state._mouseItemPresent = false;
		_state._keyboardItemPresent = false;
		_state._layout = GuiLayout{ RectF{ Rect{ _state._window.size() } } };
		_state.updateBlankTexture(_state._defaultFont);
		setButtonStyle({});
		setLabelStyle({});
		setEditStyle({});
	}

	GuiFrame::~GuiFrame() noexcept
	{
		if (_state._mouseItemKey != Key::Null && _state.captureClick(_state._mouseItemKey, false, true).second)
		{
			_state._mouseItem.clear();
			_state._mouseItemKey = Key::Null;
		}
		if (!_state._mouseItemPresent)
		{
			_state._mouseItem.clear();
			_state._mouseItemKey = Key::Null;
		}
		if (!_state._keyboardItemPresent)
		{
			_state._keyboardItem.clear();
			_state._keyboardItemCursor = 0;
		}
		_state._inputEvents.clear();
	}

	bool GuiFrame::button(std::string_view id, std::string_view text, const RectF& rect)
	{
		assert(!id.empty());
		const auto usedRect = rect == RectF{} ? _state._layout.add() : rect;
		if (usedRect.left() == usedRect.right() || usedRect.top() == usedRect.bottom())
			return false;
		bool clicked = false;
		const auto* styleState = &_state._buttonStyle._normal;
		if (_state._mouseItem == id)
		{
			assert(_state._mouseCursor);
			assert(!_state._mouseItemPresent);
			assert(_state._keyboardItem.empty());
			const auto hovered = rect.contains(*_state._mouseCursor);
			const auto released = _state.captureClick(_state._mouseItemKey, false, true).second;
			if (released)
			{
				_state._mouseItem.clear();
				_state._mouseItemKey = Key::Null;
				if (hovered)
				{
					clicked = true;
					styleState = &_state._buttonStyle._hovered;
					_state._mouseCursor.reset();
				}
			}
			else
			{
				styleState = &_state._buttonStyle._pressed;
				_state._mouseCursor.reset();
				_state._mouseItemPresent = true;
			}
		}
		else if (_state._mouseItem.empty() && _state.captureMouse(rect))
		{
			assert(!_state._mouseItemPresent);
			styleState = &_state._buttonStyle._hovered;
			if (const auto [pressed, released] = _state.captureClick(Key::Mouse1, false); pressed)
			{
				if (!released)
				{
					_state._mouseItem = id;
					_state._mouseItemPresent = true;
					_state._mouseItemKey = Key::Mouse1;
					styleState = &_state._buttonStyle._pressed;
				}
				else
					clicked = true;
				_state._keyboardItem.clear();
				_state._keyboardItemPresent = false;
				_state._keyboardItemCursor = 0;
			}
		}
		_state.updateBlankTexture(_state._buttonStyle._font);
		selectBlankTexture();
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
		if (_state._mouseItem == id)
		{
			assert(_state._mouseCursor);
			assert(!_state._mouseItemPresent);
			auto captured = rect.bound(*_state._mouseCursor);
			_state._mouseCursor.reset();
			_state._mouseItemPresent = true;
			return captured;
		}
		if (_state._mouseItem.empty())
		{
			assert(!_state._mouseItemPresent);
			if (auto maybeCaptured = _state.captureMouse(rect))
				if (const auto [pressed, released] = _state.captureClick(key, false); pressed)
				{
					if (!released)
					{
						_state._mouseItem = id;
						_state._mouseItemPresent = true;
						_state._mouseItemKey = key;
					}
					_state._keyboardItem.clear();
					_state._keyboardItemPresent = false;
					_state._keyboardItemCursor = 0;
					return maybeCaptured;
				}
		}
		return {};
	}

	std::optional<Vector2> GuiFrame::hoverArea(const RectF& rect) noexcept
	{
		return _state.captureMouse(rect);
	}

	void GuiFrame::label(std::string_view text, const RectF& rect)
	{
		if (!_state._labelStyle._font)
			return;
		const auto usedRect = rect == RectF{} ? _state._layout.add() : rect;
		if (usedRect.top() == usedRect.bottom())
			return;
		const auto fontSize = usedRect.height() * _state._buttonStyle._fontSize;
		const auto padding = (usedRect.height() - _state._buttonStyle._font->text_size(text, { 1, fontSize })._height) / 2;
		_state.updateBlankTexture(_state._labelStyle._font);
		_renderer.setColor(_state._labelStyle._textColor);
		_state._labelStyle._font->render(_renderer, usedRect.top_left() + Vector2{ padding, padding }, fontSize, text);
	}

	GuiLayout& GuiFrame::layout() noexcept
	{
		return _state._layout;
	}

	void GuiFrame::selectBlankTexture()
	{
		_renderer.setTexture(_state._blankTexture);
		if (_state._blankTexture)
			_renderer.setTextureRect(_state._blankTextureRect);
	}

	void GuiFrame::setButtonStyle(const GuiButtonStyle& style) noexcept
	{
		_state._buttonStyle = style;
		if (!_state._buttonStyle._font)
			_state._buttonStyle._font = _state._defaultFont;
	}

	void GuiFrame::setEditStyle(const GuiEditStyle& style) noexcept
	{
		_state._editStyle = style;
		if (!_state._editStyle._font)
			_state._editStyle._font = _state._defaultFont;
	}

	void GuiFrame::setLabelStyle(const GuiLabelStyle& style) noexcept
	{
		_state._labelStyle = style;
		if (!_state._labelStyle._font)
			_state._labelStyle._font = _state._defaultFont;
	}

	bool GuiFrame::stringEdit(std::string_view id, std::string& text, const RectF& rect)
	{
		assert(!id.empty());
		const auto usedRect = rect == RectF{} ? _state._layout.add() : rect;
		if (usedRect.left() == usedRect.right() || usedRect.top() == usedRect.bottom())
			return false;
		bool entered = false;
		const auto* styleState = &_state._editStyle._normal;
		bool showCursor = false;
		if (_state._mouseItem == id)
		{
			assert(_state._mouseCursor);
			assert(!_state._mouseItemPresent);
			assert(_state._keyboardItem == id);
			const auto released = _state.captureClick(_state._mouseItemKey, false, true).second;
			if (released)
			{
				_state._mouseItem.clear();
				_state._mouseItemKey = Key::Null;
				if (rect.contains(*_state._mouseCursor))
					_state._mouseCursor.reset();
			}
			else
			{
				styleState = &_state._editStyle._active;
				_state._mouseCursor.reset();
			}
		}
		else if (_state._mouseItem.empty() && _state.captureMouse(rect))
		{
			styleState = &_state._editStyle._hovered;
			if (const auto [pressed, released] = _state.captureClick(Key::Mouse1, false); pressed)
			{
				if (!released)
				{
					_state._mouseItem = id;
					_state._mouseItemPresent = true;
					_state._mouseItemKey = Key::Mouse1;
				}
				_state._keyboardItem = id;
				_state._keyboardItemCursorMark = std::chrono::steady_clock::now();
			}
		}
		if (_state._keyboardItem == id)
		{
			assert(!_state._keyboardItemPresent);
			_state._keyboardItemPresent = true;
			styleState = &_state._editStyle._active;
			showCursor = true;
			if (_state._keyboardItemCursor > text.size())
				_state._keyboardItemCursor = text.size();
			else if (_state._keyboardItemCursor < text.size())
				while (_state._keyboardItemCursor > 0 && primal::isUtf8Continuation(text[_state._keyboardItemCursor]))
					--_state._keyboardItemCursor;
			_state.captureKeyboard([&](Key key) {
				switch (key)
				{
				case Key::Enter:
				case Key::NumEnter:
					entered = true;
					[[fallthrough]];
				case Key::Escape:
					_state._keyboardItem.clear();
					_state._keyboardItemPresent = false;
					return false;
				case Key::Left:
					if (_state._keyboardItemCursor > 0)
						do
							--_state._keyboardItemCursor;
						while (_state._keyboardItemCursor > 0 && primal::isUtf8Continuation(text[_state._keyboardItemCursor]));
					break;
				case Key::Right:
					if (_state._keyboardItemCursor < text.size())
						do
							++_state._keyboardItemCursor;
						while (_state._keyboardItemCursor < text.size() && primal::isUtf8Continuation(text[_state._keyboardItemCursor]));
					break;
				default:
					break;
				}
				return true;
			});
		}
		_state.updateBlankTexture(_state._editStyle._font);
		selectBlankTexture();
		_renderer.setColor(styleState->_backgroundColor);
		_renderer.addRect(rect);
		if (_state._editStyle._font)
		{
			const auto fontSize = rect.height() * _state._editStyle._fontSize;
			const auto textPadding = (rect.height() - fontSize) / 2;
			_renderer.setColor(styleState->_textColor);
			TextCapture capture{ _state._keyboardItemCursor, 0, 0 };
			_state._editStyle._font->render(_renderer, rect.top_left() + Vector2{ textPadding, textPadding }, fontSize, text, &capture);
			if (showCursor && capture._has_cursor && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _state._keyboardItemCursorMark).count() % 1000 < 500)
			{
				_renderer.setTextureRect(_state._editStyle._font->white_rect());
				_renderer.setColor(_state._editStyle._cursorColor);
				_renderer.addRect(capture._cursor_rect);
			}
		}
		return entered;
	}
}
