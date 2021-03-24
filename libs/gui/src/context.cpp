// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/context.h>

#include "context_data.h"

namespace Yt
{
	GuiContext::GuiContext(Window& window)
		: _data{ std::make_unique<GuiContextData>(window) }
	{
	}

	GuiContext::~GuiContext() noexcept = default;

	EventCallbacks& GuiContext::eventCallbacks() noexcept
	{
		return *_data;
	}

	void GuiContext::setDefaultFont(const std::shared_ptr<const Font>& font) noexcept
	{
		_data->_defaultFont = font;
	}
}
