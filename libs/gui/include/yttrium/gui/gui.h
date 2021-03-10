// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <optional>

namespace Yt
{
	enum class Key;
	class KeyEvent;
	class Point;
	class Rect;
	class Window;

	class GuiState
	{
	public:
		GuiState(Window&);
		~GuiState() noexcept;

		void processKeyEvent(const KeyEvent&);

	private:
		const std::unique_ptr<class GuiStateData> _data;
		friend class GuiFrame;
	};

	class GuiFrame
	{
	public:
		GuiFrame(GuiState&);
		~GuiFrame() noexcept;

		bool keyPressed(Key) noexcept;
		std::optional<Point> mouseArea(const Rect&) noexcept;

	private:
		GuiStateData& _state;
	};
}
