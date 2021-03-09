// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace Yt
{
	enum class Key;
	class KeyEvent;

	class GuiState
	{
	public:
		GuiState();
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

	private:
		GuiStateData& _state;
	};
}
