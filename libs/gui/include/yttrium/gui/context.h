// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string_view>

namespace Yt
{
	class Font;
	class KeyEvent;
	class Window;

	class GuiContext
	{
	public:
		explicit GuiContext(Window&);
		~GuiContext() noexcept;

		void processKeyEvent(const KeyEvent&);
		void processTextInput(std::string_view);
		void setDefaultFont(const std::shared_ptr<const Font>&) noexcept;

	private:
		const std::unique_ptr<class GuiContextData> _data;
		friend class GuiFrame;
	};
}
