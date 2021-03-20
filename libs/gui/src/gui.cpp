// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/gui.h>

#include <yttrium/application/key.h>
#include <yttrium/application/window.h>
#include <yttrium/gui/font.h>
#include <yttrium/gui/layout.h>
#include <yttrium/gui/style.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/2d.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <string>
#include <vector>

#include <primal/utf8.hpp>

namespace
{
	constexpr Yt::RectF relativeHeightInRect(const Yt::RectF& rect, float relativeHeight) noexcept
	{
		const auto padding = rect.height() * (1 - relativeHeight) / 2;
		return { rect.top_left() + padding, rect.bottom_right() - padding };
	}

	constexpr Yt::RectF sizeInRect(const Yt::RectF& rect, const Yt::SizeF& size) noexcept
	{
		const auto yPadding = (rect.height() - size._height) / 2;
		const auto xPadding = std::max(yPadding, (rect.width() - size._width) / 2);
		return { rect.top_left() + Yt::Vector2{ xPadding, yPadding }, rect.bottom_right() - Yt::Vector2{ xPadding, yPadding } };
	}
}

namespace Yt
{
	class GuiStateData
	{
	public:
		static constexpr uint16_t kPayloadMask = 0x00ff;
		static constexpr uint16_t kShiftFlag = 0x0100;
		static constexpr uint16_t kPressedFlag = 0x1000;
		static constexpr uint16_t kAutorepeatFlag = 0x2000;
		static constexpr uint16_t kTextFlag = 0x4000;
		static constexpr uint16_t kProcessedFlag = 0x8000;
		static constexpr uint16_t kKeySearchMask = kPayloadMask | kTextFlag | kProcessedFlag;

		struct KeyboardItem
		{
			std::string _id;
			bool _present = false;
			size_t _cursor = 0;
			std::chrono::steady_clock::time_point _cursorMark;
			size_t _selectionOffset = 0;
			size_t _selectionSize = 0;

			constexpr void adjustToText(std::string_view text) noexcept
			{
				if (_cursor > text.size())
					_cursor = text.size();
				else if (_cursor < text.size())
					while (_cursor > 0 && primal::isUtf8Continuation(text[_cursor]))
						--_cursor;
				if (_selectionOffset > _cursor)
					_selectionOffset = _cursor;
				if (const auto maxSelectionSize = text.size() - _selectionOffset; maxSelectionSize < _selectionSize)
					_selectionSize = maxSelectionSize;
			}

			void onBackspace(std::string& text) noexcept
			{
				assert(_cursor <= text.size());
				size_t count = 0;
				if (_selectionSize > 0)
				{
					assert(_cursor == _selectionOffset
						|| _cursor == _selectionOffset + _selectionSize);
					count = std::exchange(_selectionSize, 0);
					_cursor = _selectionOffset;
				}
				else if (_cursor > 0)
				{
					count = leftStep(text);
					_cursor -= count;
				}
				else
					return;
				text.erase(_cursor, count);
				_cursorMark = std::chrono::steady_clock::now();
			}

			void onDelete(std::string& text) noexcept
			{
				assert(_cursor <= text.size());
				size_t count = 0;
				if (_selectionSize > 0)
				{
					assert(_cursor == _selectionOffset
						|| _cursor == _selectionOffset + _selectionSize);
					count = std::exchange(_selectionSize, 0);
					_cursor = _selectionOffset;
				}
				else if (_cursor < text.size())
					count = rightStep(text);
				else
					return;
				text.erase(_cursor, count);
				_cursorMark = std::chrono::steady_clock::now();
			}

			void onEnd(std::string_view text, bool shift) noexcept
			{
				if (_cursor < text.size())
				{
					if (shift)
					{
						if (!_selectionSize)
							_selectionOffset = _cursor;
						else if (_cursor == _selectionOffset)
							_selectionOffset += _selectionSize;
						_selectionSize = text.size() - _selectionOffset;
					}
					_cursor = text.size();
					_cursorMark = std::chrono::steady_clock::now();
				}
				if (!shift)
					_selectionSize = 0;
			}

			void onHome(bool shift) noexcept
			{
				if (_cursor > 0)
				{
					if (shift)
					{
						if (_selectionSize > 0 && _selectionOffset < _cursor)
							_selectionSize = _selectionOffset;
						else
							_selectionSize += _cursor;
						_selectionOffset = 0;
					}
					_cursor = 0;
					_cursorMark = std::chrono::steady_clock::now();
				}
				if (!shift)
					_selectionSize = 0;
			}

			void onLeft(std::string_view text, bool shift) noexcept
			{
				if (_cursor > 0)
				{
					const auto step = leftStep(text);
					assert(step > 0 && step <= _cursor);
					_cursor -= step;
					_cursorMark = std::chrono::steady_clock::now();
					if (shift)
					{
						if (_selectionSize > 0 && _selectionOffset <= _cursor)
							_selectionSize -= step;
						else
						{
							_selectionSize += step;
							_selectionOffset = _cursor;
						}
					}
				}
				if (!shift)
					_selectionSize = 0;
			}

			void onPaste(std::string& text, std::string_view paste)
			{
				if (_selectionSize > 0)
				{
					text.erase(_selectionOffset, _selectionSize);
					_cursor = _selectionOffset;
					_selectionSize = 0;
				}
				text.insert(_cursor, paste);
				_cursor += paste.size();
				_cursorMark = std::chrono::steady_clock::now();
			}

			void onRight(std::string_view text, bool shift) noexcept
			{
				if (_cursor < text.size())
				{
					const auto step = rightStep(text);
					assert(step > 0 && step <= text.size() - _cursor);
					if (shift)
					{
						if (_selectionSize > 0 && _selectionOffset == _cursor)
						{
							_selectionSize -= step;
							_selectionOffset += step;
						}
						else
						{
							_selectionOffset = _cursor - _selectionSize;
							_selectionSize += step;
						}
					}
					_cursor += step;
					_cursorMark = std::chrono::steady_clock::now();
				}
				if (!shift)
					_selectionSize = 0;
			}

		private:
			constexpr size_t leftStep(std::string_view text) const noexcept
			{
				assert(_cursor > 0);
				auto offset = _cursor;
				do
					--offset;
				while (offset > 0 && primal::isUtf8Continuation(text[offset]));
				return _cursor - offset;
			}

			constexpr size_t rightStep(std::string_view text) const noexcept
			{
				assert(_cursor < text.size());
				auto offset = _cursor;
				do
					++offset;
				while (offset < text.size() && primal::isUtf8Continuation(text[offset]));
				return offset - _cursor;
			}
		};

		Window& _window;
		std::vector<uint16_t> _inputEvents;
		std::vector<std::string> _textInputs;
		std::optional<Vector2> _mouseCursor;
		std::string _mouseItem;
		bool _mouseItemPresent = false;
		Key _mouseItemKey = Key::Null;
		KeyboardItem _keyboardItem;
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

		void captureKeyboard(std::function<bool(Key, bool)>&& keyCallback, std::function<void(std::string_view)>&& textCallback)
		{
			assert(!_keyboardItem._id.empty());
			for (auto& event : _inputEvents)
			{
				if (event & kProcessedFlag)
					continue;
				event |= kProcessedFlag;
				if (event & kTextFlag)
					textCallback(_textInputs[event & kPayloadMask]);
				else if ((event & kPressedFlag) && !keyCallback(static_cast<Key>(event & kPayloadMask), event & kShiftFlag))
					break;
			}
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
				_blankTextureRect = font->textureRect(Font::Graphics::WhiteRect);
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
			if (event._modifiers & KeyEvent::Modifier::Shift)
				encodedEvent |= GuiStateData::kShiftFlag;
		}
		_data->_inputEvents.emplace_back(encodedEvent);
	}

	void GuiState::processTextInput(std::string_view text)
	{
		const auto index = _data->_textInputs.size();
		if (index >= GuiStateData::kPayloadMask)
			return;
		_data->_inputEvents.reserve(_data->_inputEvents.size() + 1);
		_data->_textInputs.emplace_back(text);
		_data->_inputEvents.emplace_back(static_cast<uint16_t>(GuiStateData::kTextFlag | index));
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
		_state._keyboardItem._present = false;
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
		if (!_state._keyboardItem._present)
			_state._keyboardItem._id.clear();
		_state._inputEvents.clear();
		_state._textInputs.clear();
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
			assert(_state._keyboardItem._id.empty());
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
				_state._keyboardItem._id.clear();
			}
		}
		_state.updateBlankTexture(_state._buttonStyle._font);
		selectBlankTexture();
		_renderer.setColor(styleState->_backgroundColor);
		_renderer.addRect(rect);
		if (_state._buttonStyle._font)
		{
			const auto textHeight = rect.height() * _state._buttonStyle._fontSize;
			const auto textWidth = _state._buttonStyle._font->textWidth(text, textHeight);
			_renderer.setColor(styleState->_textColor);
			_state._buttonStyle._font->render(_renderer, ::sizeInRect(rect, { textWidth, textHeight }), text);
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
					_state._keyboardItem._id.clear();
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
		auto usedRect = rect == RectF{} ? _state._layout.add() : rect;
		if (usedRect.top() == usedRect.bottom())
			return;
		if (usedRect.left() == usedRect.right())
			usedRect.set_right(_renderer.viewportSize()._width);
		_state.updateBlankTexture(_state._labelStyle._font);
		_renderer.setColor(_state._labelStyle._textColor);
		_state._labelStyle._font->render(_renderer, ::relativeHeightInRect(usedRect, _state._buttonStyle._fontSize), text);
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
		bool active = false;
		if (_state._mouseItem == id)
		{
			assert(_state._mouseCursor);
			assert(!_state._mouseItemPresent);
			assert(_state._keyboardItem._id == id);
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
				_state._keyboardItem._id = id;
				_state._keyboardItem._cursor = 0;
				_state._keyboardItem._cursorMark = std::chrono::steady_clock::now();
				_state._keyboardItem._selectionSize = 0;
			}
		}
		if (_state._keyboardItem._id == id)
		{
			assert(!_state._keyboardItem._present);
			_state._keyboardItem._present = true;
			styleState = &_state._editStyle._active;
			active = true;
			_state._keyboardItem.adjustToText(text);
			_state.captureKeyboard(
				[&](Key key, bool shift) {
					switch (key)
					{
					case Key::Enter:
					case Key::NumEnter:
						entered = true;
						[[fallthrough]];
					case Key::Escape:
						_state._keyboardItem._id.clear();
						active = false;
						return false;
					case Key::Left:
						_state._keyboardItem.onLeft(text, shift);
						break;
					case Key::Right:
						_state._keyboardItem.onRight(text, shift);
						break;
					case Key::Backspace:
						_state._keyboardItem.onBackspace(text);
						break;
					case Key::Delete:
						_state._keyboardItem.onDelete(text);
						break;
					case Key::Home:
						_state._keyboardItem.onHome(shift);
						break;
					case Key::End:
						_state._keyboardItem.onEnd(text, shift);
						break;
					default:
						break;
					}
					return true;
				},
				[&](std::string_view paste) {
					_state._keyboardItem.onPaste(text, paste);
				});
		}
		_state.updateBlankTexture(_state._editStyle._font);
		selectBlankTexture();
		_renderer.setColor(styleState->_backgroundColor);
		_renderer.addRect(rect);
		if (_state._editStyle._font)
		{
			const auto textRect = ::relativeHeightInRect(rect, _state._editStyle._fontSize);
			Font::TextCapture capture{ _state._keyboardItem._cursor, _state._keyboardItem._selectionOffset, _state._keyboardItem._selectionSize };
			const auto textWidth = _state._editStyle._font->textWidth(text, textRect.height(), &capture);
			if (active && capture._selectionRange)
			{
				const auto selectionLeft = textRect.left() + capture._selectionRange->first;
				if (selectionLeft < textRect.right())
				{
					const auto selectionRight = std::min(textRect.left() + capture._selectionRange->second, textRect.right());
					_renderer.setColor(_state._editStyle._selectionColor);
					_renderer.addBorderlessRect({ { selectionLeft, textRect.top() }, Vector2{ selectionRight, textRect.bottom() } });
				}
			}
			_renderer.setColor(styleState->_textColor);
			_state._editStyle._font->render(_renderer, textRect, text);
			if (active && capture._cursorPosition)
			{
				const auto cursorX = textRect.left() + *capture._cursorPosition;
				if (cursorX < textRect.right() && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _state._keyboardItem._cursorMark).count() % 1000 < 500)
				{
					_renderer.setTextureRect(_state._editStyle._font->textureRect(Font::Graphics::WhiteRect));
					_renderer.setColor(_state._editStyle._cursorColor);
					_renderer.addRect({ { cursorX, textRect.top() }, Vector2{ std::min(cursorX + 2, textRect.right()), textRect.bottom() } });
				}
			}
		}
		return entered;
	}
}
