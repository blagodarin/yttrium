//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "application.h"

#include <yttrium/exceptions.h>

#include <clocale>

namespace Yttrium
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
