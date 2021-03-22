// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "context_data.h"

#include <yttrium/gui/layout.h>

namespace
{
	Yt::GuiLayout kNullLayout;
}

namespace Yt
{
	GuiContextData::GuiContextData(Window& window) noexcept
		: _window{ window }
		, _layout{ &kNullLayout }
	{
	}
}
