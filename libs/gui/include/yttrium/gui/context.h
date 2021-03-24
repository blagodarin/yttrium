// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace Yt
{
	class EventCallbacks;
	class Font;
	class Window;

	class GuiContext
	{
	public:
		explicit GuiContext(Window&);
		~GuiContext() noexcept;

		EventCallbacks& eventCallbacks() noexcept;
		void setDefaultFont(const std::shared_ptr<const Font>&) noexcept;

	private:
		const std::unique_ptr<class GuiContextData> _data;
		friend class GuiFrame;
	};
}
