// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

namespace Yt
{
	struct ApplicationID
	{
		intptr_t _application = 0;
	};

	struct WindowID : ApplicationID
	{
		intptr_t _window = 0;
		WindowID(intptr_t application, intptr_t window)
			: ApplicationID{ application }, _window{ window } {}
	};
}
