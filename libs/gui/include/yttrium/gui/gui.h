// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/gui/layout.h>

#include <seir_graphics/rectf.hpp>

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
	class Renderer2D;

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

		bool addButton(std::string_view id, std::string_view text, const seir::RectF& = {});
		std::optional<seir::Vec2> addDragArea(std::string_view id, const seir::RectF&, Key);
		std::optional<seir::Vec2> addHoverArea(const seir::RectF&) noexcept;
		void addLabel(std::string_view text, GuiAlignment = GuiAlignment::Left, const seir::RectF& = {});
		bool addStringEdit(std::string_view id, std::string& text, const seir::RectF& = {});
		void putDefaultFocus() noexcept;
		Renderer2D& renderer() noexcept { return _renderer; }
		void selectBlankTexture();
		void setButtonStyle(const GuiButtonStyle&) noexcept;
		void setEditStyle(const GuiEditStyle&) noexcept;
		void setLabelStyle(const GuiLabelStyle&) noexcept;
		bool takeAnyKeyPress() noexcept;
		bool takeKeyPress(Key) noexcept;
		std::optional<bool> takeKeyState(Key) noexcept;
		std::optional<seir::Vec2> takeMouseCursor() noexcept;

	private:
		GuiContextData& _context;
		Renderer2D& _renderer;
		friend GuiLayout;
	};
}
