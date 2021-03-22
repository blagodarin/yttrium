// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/gui/layout.h>

#include <memory>
#include <optional>
#include <string_view>

namespace Yt
{
	class Font;
	struct GuiButtonStyle;
	class GuiContext;
	class GuiContextData;
	struct GuiEditStyle;
	struct GuiLabelStyle;
	class GuiLayout;
	enum class Key;
	class RectF;
	class Renderer2D;
	class Vector2;

	enum class GuiAlignment
	{
		Left,
		Center,
		Right,
	};

	class GuiFrame
	{
	public:
		explicit GuiFrame(GuiContext&, Renderer2D&);
		~GuiFrame() noexcept;

		bool addButton(std::string_view id, std::string_view text, const RectF& = {});
		std::optional<Vector2> addDragArea(std::string_view id, const RectF&, Key);
		std::optional<Vector2> addHoverArea(const RectF&) noexcept;
		void addLabel(std::string_view text, GuiAlignment = GuiAlignment::Left, const RectF& = {});
		bool addStringEdit(std::string_view id, std::string& text, const RectF& = {});
		std::optional<Vector2> takeMouseCursor() noexcept;
		bool captureKeyDown(Key) noexcept;
		Renderer2D& renderer() noexcept { return _renderer; }
		void selectBlankTexture();
		void setButtonStyle(const GuiButtonStyle&) noexcept;
		void setEditStyle(const GuiEditStyle&) noexcept;
		void setLabelStyle(const GuiLabelStyle&) noexcept;

	private:
		GuiContextData& _context;
		Renderer2D& _renderer;
		friend GuiLayout;
	};
}
