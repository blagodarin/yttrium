// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/color.h>
#include <yttrium/math/rect.h>

#include <memory>
#include <optional>
#include <string_view>

namespace Yt
{
	class Font;
	struct GuiButtonStyle;
	struct GuiLabelStyle;
	class GuiLayout;
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

	class GuiFrame
	{
	public:
		explicit GuiFrame(GuiState&, Renderer2D&);
		~GuiFrame() noexcept;

		bool button(std::string_view id, std::string_view text, const RectF& = {});
		bool captureKeyDown(Key) noexcept;
		std::optional<Vector2> dragArea(std::string_view id, const RectF&, Key);
		std::optional<Vector2> hoverArea(const RectF&) noexcept;
		void label(std::string_view, const RectF& = {});
		GuiLayout& layout() noexcept;
		Renderer2D& renderer() noexcept { return _renderer; }
		void setButtonStyle(const GuiButtonStyle&) noexcept;
		void setLabelStyle(const GuiLabelStyle&) noexcept;

	private:
		GuiStateData& _state;
		Renderer2D& _renderer;
	};
}
