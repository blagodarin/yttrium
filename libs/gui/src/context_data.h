// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/application/application.h>
#include <yttrium/application/event.h>
#include <yttrium/application/window.h>
#include <yttrium/geometry/rect.h>
#include <yttrium/gui/font.h>
#include <yttrium/gui/style.h>

#include <primal/utf8.hpp>

#include <cassert>
#include <chrono>
#include <functional>
#include <optional>

namespace Yt
{
	class GuiLayout;

	class GuiContextData : public EventCallbacks
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

			void setFocus(std::string_view id)
			{
				_id = id;
				_present = false;
				_cursor = std::numeric_limits<size_t>::max();
				_cursorMark = std::chrono::steady_clock::now();
				_selectionOffset = 0;
				_selectionSize = _cursor;
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
		Vector2 _mouseCursor{ 0, 0 };
		bool _mouseCursorTaken = false;
		bool _mouseHoverTaken = false;
		std::string _mouseItem;
		bool _mouseItemPresent = false;
		Key _mouseItemKey = Key::None;
		KeyboardItem _keyboardItem;
		GuiButtonStyle _buttonStyle;
		GuiEditStyle _editStyle;
		GuiLabelStyle _labelStyle;
		std::shared_ptr<const Font> _defaultFont;
		std::shared_ptr<const Texture2D> _blankTexture;
		RectF _blankTextureRect;
		GuiLayout* _layout = nullptr;
		std::array<uint8_t, 256> _keyStates{};
		bool _focusExpected = false;

		static constexpr uint8_t kKeyStateTaken = 0x80;
		static constexpr uint8_t kKeyStatePressed = 0x01;

		explicit GuiContextData(Window&) noexcept;

		std::pair<unsigned, bool> captureClick(Key key, bool autorepeat, bool release = false) noexcept
		{
			const auto i = std::find_if(_inputEvents.begin(), _inputEvents.end(), [key](const auto event) {
				return key == Key::None ? !(event & (kTextFlag | kProcessedFlag)) : (event & kKeySearchMask) == static_cast<uint8_t>(key);
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
				if ((*j & kKeySearchMask) == (*i & kPayloadMask))
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

		RectF layoutRect() noexcept;

		std::optional<Vector2> takeMouseCursor(const RectF& rect) noexcept
		{
			if (_mouseCursorTaken || !rect.contains(_mouseCursor))
				return {};
			_mouseCursorTaken = true;
			_mouseHoverTaken = true;
			return _mouseCursor;
		}

		std::optional<Vector2> takeMouseHover(const RectF& rect) noexcept
		{
			if (_mouseHoverTaken || !rect.contains(_mouseCursor))
				return {};
			_mouseHoverTaken = true;
			return _mouseCursor;
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

	private:
		void onWindowMouseMove(const Window&, int dx, int dy) override;
		void onWindowKeyEvent(const Window&, const KeyEvent&) override;
		void onWindowTextInput(const Window&, std::string_view) override;
	};
}
