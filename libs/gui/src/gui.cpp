// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/gui.h>

#include <yttrium/gui/context.h>
#include <yttrium/gui/layout.h>
#include <yttrium/renderer/2d.h>

#include "context_data.h"

#include <algorithm>
#include <string>
#include <vector>

namespace
{
	constexpr Yt::RectF relativeHeightInRect(const Yt::RectF& rect, float relativeHeight) noexcept
	{
		const auto padding = rect.height() * (1 - relativeHeight) / 2;
		return { rect.topLeft() + padding, rect.bottomRight() - padding };
	}

	constexpr Yt::RectF sizeInRect(const Yt::RectF& rect, const Yt::SizeF& size) noexcept
	{
		const auto yPadding = (rect.height() - size._height) / 2;
		const auto xPadding = std::max(yPadding, (rect.width() - size._width) / 2);
		return { rect.topLeft() + Yt::Vector2{ xPadding, yPadding }, rect.bottomRight() - Yt::Vector2{ xPadding, yPadding } };
	}
}

namespace Yt
{
	GuiFrame::GuiFrame(GuiContext& context, Renderer2D& renderer)
		: _context{ *context._data }
		, _renderer{ renderer }
	{
		_context._mouseCursor = Vector2{ _context._window.cursor() };
		_context._mouseCursorTaken = false;
		_context._mouseHoverTaken = false;
		_context._mouseItemPresent = false;
		_context._keyboardItem._present = false;
		_context.updateBlankTexture(_context._defaultFont);
		setButtonStyle({});
		setLabelStyle({});
		setEditStyle({});
	}

	GuiFrame::~GuiFrame() noexcept
	{
		if (_context._mouseItemKey != Key::None && _context.captureClick(_context._mouseItemKey, false, true).second)
		{
			_context._mouseItem.clear();
			_context._mouseItemKey = Key::None;
		}
		if (!_context._mouseItemPresent)
		{
			_context._mouseItem.clear();
			_context._mouseItemKey = Key::None;
		}
		if (!_context._keyboardItem._present)
			_context._keyboardItem._id.clear();
		_context._inputEvents.clear();
		_context._textInputs.clear();
		for (auto& keyState : _context._keyStates)
			keyState &= static_cast<uint8_t>(~GuiContextData::kKeyStateTaken);
	}

	bool GuiFrame::addButton(std::string_view id, std::string_view text, const RectF& rect)
	{
		assert(!id.empty());
		const auto widgetRect = rect.null() ? _context.layoutRect() : rect;
		if (widgetRect.empty())
			return false;
		bool clicked = false;
		const auto* styleState = &_context._buttonStyle._normal;
		if (_context._mouseItem == id)
		{
			assert(!_context._mouseHoverTaken);
			assert(!_context._mouseItemPresent);
			assert(_context._keyboardItem._id.empty());
			const auto hovered = widgetRect.contains(_context._mouseCursor);
			const auto released = _context.captureClick(_context._mouseItemKey, false, true).second;
			if (released)
			{
				_context._mouseItem.clear();
				_context._mouseItemKey = Key::None;
				if (hovered)
				{
					clicked = true;
					styleState = &_context._buttonStyle._hovered;
					_context._mouseHoverTaken = true;
				}
			}
			else
			{
				styleState = &_context._buttonStyle._pressed;
				_context._mouseHoverTaken = true;
				_context._mouseItemPresent = true;
			}
		}
		else if (_context._mouseItem.empty() && _context.takeMouseHover(widgetRect))
		{
			assert(!_context._mouseItemPresent);
			styleState = &_context._buttonStyle._hovered;
			if (const auto [pressed, released] = _context.captureClick(Key::Mouse1, false); pressed)
			{
				if (!released)
				{
					_context._mouseItem = id;
					_context._mouseItemPresent = true;
					_context._mouseItemKey = Key::Mouse1;
					styleState = &_context._buttonStyle._pressed;
				}
				else
					clicked = true;
				_context._keyboardItem._id.clear();
			}
		}
		_context.updateBlankTexture(_context._buttonStyle._font);
		selectBlankTexture();
		_renderer.setColor(styleState->_backgroundColor);
		_renderer.addRect(widgetRect);
		if (_context._buttonStyle._font)
		{
			const auto textHeight = widgetRect.height() * _context._buttonStyle._fontSize;
			const auto textWidth = _context._buttonStyle._font->textWidth(text, textHeight);
			_renderer.setColor(styleState->_textColor);
			_context._buttonStyle._font->render(_renderer, ::sizeInRect(widgetRect, { textWidth, textHeight }), text);
		}
		return clicked;
	}

	std::optional<Vector2> GuiFrame::addDragArea(std::string_view id, const RectF& rect, Key key)
	{
		assert(!id.empty());
		if (_context._mouseItem == id)
		{
			assert(!_context._mouseHoverTaken);
			assert(!_context._mouseItemPresent);
			auto captured = rect.bound(_context._mouseCursor);
			_context._mouseHoverTaken = true;
			_context._mouseItemPresent = true;
			return captured;
		}
		if (_context._mouseItem.empty())
		{
			assert(!_context._mouseItemPresent);
			if (auto maybeHover = _context.takeMouseHover(rect))
				if (const auto [pressed, released] = _context.captureClick(key, false); pressed)
				{
					if (!released)
					{
						_context._mouseItem = id;
						_context._mouseItemPresent = true;
						_context._mouseItemKey = key;
					}
					_context._keyboardItem._id.clear();
					return maybeHover;
				}
		}
		return {};
	}

	std::optional<Vector2> GuiFrame::addHoverArea(const RectF& rect) noexcept
	{
		return _context.takeMouseHover(rect);
	}

	void GuiFrame::addLabel(std::string_view text, GuiAlignment alignment, const RectF& rect)
	{
		if (!_context._labelStyle._font)
			return;
		auto textRect = rect.null() ? _context.layoutRect() : rect;
		if (textRect.top() >= textRect.bottom())
			return;
		const auto verticalPadding = textRect.height() * (1 - _context._buttonStyle._fontSize) / 2;
		textRect._top += verticalPadding;
		textRect._bottom -= verticalPadding;
		if (textRect.left() == textRect.right())
		{
			if (alignment == GuiAlignment::Left || alignment == GuiAlignment::Center)
				textRect._right = _renderer.viewportSize()._width;
			if (alignment == GuiAlignment::Center || alignment == GuiAlignment::Right)
				textRect._left = 0;
		}
		if (const auto textWidth = _context._editStyle._font->textWidth(text, textRect.height()); textWidth < textRect.width())
		{
			if (alignment == GuiAlignment::Center)
			{
				const auto horizontalPadding = (textRect.width() - textWidth) / 2;
				textRect._left += horizontalPadding;
				textRect._right -= horizontalPadding;
			}
			else if (alignment == GuiAlignment::Right)
				textRect._left = textRect._right - textWidth;
		}
		_context.updateBlankTexture(_context._labelStyle._font);
		_renderer.setColor(_context._labelStyle._textColor);
		_context._labelStyle._font->render(_renderer, textRect, text);
	}

	bool GuiFrame::addStringEdit(std::string_view id, std::string& text, const RectF& rect)
	{
		assert(!id.empty());
		const auto widgetRect = rect.null() ? _context.layoutRect() : rect;
		if (widgetRect.empty())
			return false;
		bool entered = false;
		const auto* styleState = &_context._editStyle._normal;
		bool active = false;
		if (_context._mouseItem == id)
		{
			assert(!_context._mouseHoverTaken);
			assert(!_context._mouseItemPresent);
			assert(_context._keyboardItem._id == id);
			const auto released = _context.captureClick(_context._mouseItemKey, false, true).second;
			if (released)
			{
				_context._mouseItem.clear();
				_context._mouseItemKey = Key::None;
				if (widgetRect.contains(_context._mouseCursor))
					_context._mouseHoverTaken = true;
			}
			else
			{
				styleState = &_context._editStyle._active;
				_context._mouseHoverTaken = true;
			}
		}
		else if (_context._mouseItem.empty() && _context.takeMouseHover(widgetRect))
		{
			styleState = &_context._editStyle._hovered;
			if (const auto [pressed, released] = _context.captureClick(Key::Mouse1, false); pressed)
			{
				if (!released)
				{
					_context._mouseItem = id;
					_context._mouseItemPresent = true;
					_context._mouseItemKey = Key::Mouse1;
				}
				_context._keyboardItem._id = id;
				_context._keyboardItem._cursor = 0;
				_context._keyboardItem._cursorMark = std::chrono::steady_clock::now();
				_context._keyboardItem._selectionSize = 0;
			}
		}
		if (_context._keyboardItem._id == id)
		{
			assert(!_context._keyboardItem._present);
			_context._keyboardItem._present = true;
			styleState = &_context._editStyle._active;
			active = true;
			_context._keyboardItem.adjustToText(text);
			_context.captureKeyboard(
				[&](Key key, bool shift) {
					switch (key)
					{
					case Key::Enter:
					case Key::NumEnter:
						entered = true;
						[[fallthrough]];
					case Key::Escape:
						_context._keyboardItem._id.clear();
						active = false;
						return false;
					case Key::Left:
						_context._keyboardItem.onLeft(text, shift);
						break;
					case Key::Right:
						_context._keyboardItem.onRight(text, shift);
						break;
					case Key::Backspace:
						_context._keyboardItem.onBackspace(text);
						break;
					case Key::Delete:
						_context._keyboardItem.onDelete(text);
						break;
					case Key::Home:
						_context._keyboardItem.onHome(shift);
						break;
					case Key::End:
						_context._keyboardItem.onEnd(text, shift);
						break;
					default:
						break;
					}
					return true;
				},
				[&](std::string_view paste) {
					_context._keyboardItem.onPaste(text, paste);
				});
		}
		_context.updateBlankTexture(_context._editStyle._font);
		selectBlankTexture();
		_renderer.setColor(styleState->_backgroundColor);
		_renderer.addRect(widgetRect);
		if (_context._editStyle._font)
		{
			const auto textRect = ::relativeHeightInRect(widgetRect, _context._editStyle._fontSize);
			Font::TextCapture capture{ _context._keyboardItem._cursor, _context._keyboardItem._selectionOffset, _context._keyboardItem._selectionSize };
			_context._editStyle._font->textWidth(text, textRect.height(), &capture);
			if (active && capture._selectionRange)
			{
				const auto selectionLeft = textRect.left() + capture._selectionRange->first;
				if (selectionLeft < textRect.right())
				{
					const auto selectionRight = std::min(textRect.left() + capture._selectionRange->second, textRect.right());
					_renderer.setColor(_context._editStyle._selectionColor);
					_renderer.addBorderlessRect({ { selectionLeft, textRect.top() }, Vector2{ selectionRight, textRect.bottom() } });
				}
			}
			_renderer.setColor(styleState->_textColor);
			_context._editStyle._font->render(_renderer, textRect, text);
			if (active && capture._cursorPosition)
			{
				const auto cursorX = textRect.left() + *capture._cursorPosition;
				if (cursorX < textRect.right() && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _context._keyboardItem._cursorMark).count() % 1000 < 500)
				{
					_renderer.setTextureRect(_context._editStyle._font->textureRect(Font::Graphics::WhiteRect));
					_renderer.setColor(_context._editStyle._cursorColor);
					_renderer.addRect({ { cursorX, textRect.top() }, Vector2{ std::min(cursorX + 2, textRect.right()), textRect.bottom() } });
				}
			}
		}
		return entered;
	}

	std::optional<Vector2> GuiFrame::takeMouseCursor() noexcept
	{
		return _context.takeMouseCursor(RectF{ _renderer.viewportSize() });
	}

	bool GuiFrame::takeAnyKeyPress() noexcept
	{
		return takeKeyPress(Key::None);
	}

	bool GuiFrame::takeKeyPress(Key key) noexcept
	{
		return _context.captureClick(key, false).first > 0;
	}

	std::optional<bool> GuiFrame::takeKeyState(Key key) noexcept
	{
		auto& keyState = _context._keyStates[static_cast<uint8_t>(key)];
		if (keyState & GuiContextData::kKeyStateTaken)
			return {};
		keyState |= GuiContextData::kKeyStateTaken;
		return static_cast<bool>(keyState & GuiContextData::kKeyStatePressed);
	}

	void GuiFrame::selectBlankTexture()
	{
		_renderer.setTexture(_context._blankTexture);
		if (_context._blankTexture)
			_renderer.setTextureRect(_context._blankTextureRect);
	}

	void GuiFrame::setButtonStyle(const GuiButtonStyle& style) noexcept
	{
		_context._buttonStyle = style;
		if (!_context._buttonStyle._font)
			_context._buttonStyle._font = _context._defaultFont;
	}

	void GuiFrame::setEditStyle(const GuiEditStyle& style) noexcept
	{
		_context._editStyle = style;
		if (!_context._editStyle._font)
			_context._editStyle._font = _context._defaultFont;
	}

	void GuiFrame::setLabelStyle(const GuiLabelStyle& style) noexcept
	{
		_context._labelStyle = style;
		if (!_context._labelStyle._font)
			_context._labelStyle._font = _context._defaultFont;
	}
}
