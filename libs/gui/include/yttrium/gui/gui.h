// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <optional>
#include <string_view>

namespace Yt
{
	enum class Key;
	class KeyEvent;
	class RectF;
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

	class GuiFrame
	{
	public:
		explicit GuiFrame(GuiState&);
		~GuiFrame() noexcept;

		bool captureKeyDown(Key) noexcept;
		std::optional<Vector2> dragArea(std::string_view id, const RectF&, Key);
		std::optional<Vector2> hoverArea(const RectF&) noexcept;

	private:
		GuiStateData& _state;
	};
}
