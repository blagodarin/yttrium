// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/color.h>

#include <memory>
#include <optional>
#include <string_view>

namespace Yt
{
	class Font;
	enum class Key;
	class KeyEvent;
	class RectF;
	class Renderer2D;
	class Vector2;
	class Window;

	class GuiState
	{
	public:
		explicit GuiState(Window&);
		~GuiState() noexcept;

		void processKeyEvent(const KeyEvent&);
		void setDefaultFont(const std::shared_ptr<const Font>&) noexcept;

	private:
		const std::unique_ptr<class GuiStateData> _data;
		friend class GuiFrame;
	};

	struct GuiButtonStyle
	{
		struct State
		{
			Bgra32 _backgroundColor;
			Bgra32 _textColor;
			constexpr State(Bgra32 backgroundColor, Bgra32 textColor) noexcept
				: _backgroundColor{ backgroundColor }, _textColor{ textColor } {}
		};
		std::shared_ptr<const Font> _font;
		float _fontSize = .75f;
		State _normal{ Bgra32::grayscale(0x88, 224), Bgra32::black() };
		State _hovered{ Bgra32::grayscale(0xdd, 224), Bgra32::grayscale(0x11) };
		State _pressed{ Bgra32::white(224), Bgra32::grayscale(0x22) };
	};

	class GuiFrame
	{
	public:
		explicit GuiFrame(GuiState&, Renderer2D&);
		~GuiFrame() noexcept;

		bool button(std::string_view id, std::string_view text, const RectF&);
		bool captureKeyDown(Key) noexcept;
		std::optional<Vector2> dragArea(std::string_view id, const RectF&, Key);
		std::optional<Vector2> hoverArea(const RectF&) noexcept;
		Renderer2D& renderer() noexcept { return _renderer; }
		void setButtonStyle(const GuiButtonStyle&) noexcept;

	private:
		GuiStateData& _state;
		Renderer2D& _renderer;
	};
}
