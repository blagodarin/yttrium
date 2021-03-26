// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "application.h"

#include <yttrium/base/exceptions.h>

#include <clocale>

namespace Yt
{
	NativeApplication::NativeApplication()
	{
		_display.reset(::XOpenDisplay(nullptr));
		if (!_display)
			throw InitializationError{ "XOpenDisplay failed" };

		_screen = DefaultScreen(_display.get());

		const auto locale = std::setlocale(LC_ALL, "");
		const auto modifiers = ::XSetLocaleModifiers("@im=none");
		_input_method.reset(::XOpenIM(_display, nullptr, nullptr, nullptr));
		std::setlocale(LC_ALL, locale);
		::XSetLocaleModifiers(modifiers);
		if (!_input_method)
			throw InitializationError{ "XOpenIM failed" };
	}
}
