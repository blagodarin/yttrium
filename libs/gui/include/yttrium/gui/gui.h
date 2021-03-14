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

	private:
		const std::unique_ptr<class GuiStateData> _data;
		friend class GuiFrame;
	};

	struct GuiButtonStyle
	{
		struct State
		{
			Bgra32 _backgroundColor;
			constexpr State(Bgra32 backgroundColor) noexcept
				: _backgroundColor{ backgroundColor } {}
		};
		State _normal{ Bgra32::grayscale(128, 224) };
		State _hovered{ Bgra32::grayscale(192, 224) };
		State _pressed{ Bgra32::white(224) };
	};

	class GuiFrame
	{
	public:
		explicit GuiFrame(GuiState&, Renderer2D&);
		~GuiFrame() noexcept;

		bool button(std::string_view id, const RectF&);
		bool captureKeyDown(Key) noexcept;
		std::optional<Vector2> dragArea(std::string_view id, const RectF&, Key);
		std::optional<Vector2> hoverArea(const RectF&) noexcept;
		Renderer2D& renderer() noexcept { return _renderer; }
		void setButtonStyle(const GuiButtonStyle&);

	private:
		GuiStateData& _state;
		Renderer2D& _renderer;
	};
}
