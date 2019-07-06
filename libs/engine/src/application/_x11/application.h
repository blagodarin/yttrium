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

#pragma once

#include <yttrium/memory/smart_ptr.h>

#include <type_traits>

#include <X11/Xlib.h>

namespace Yttrium
{
	class NativeApplication
	{
	public:
		NativeApplication();

		::Display* display() const noexcept { return _display; }
		XIM input_method() const noexcept { return _input_method; }
		int screen() const noexcept { return _screen; }

	private:
		SmartPtr<::Display, ::XCloseDisplay> _display;
		int _screen = 0;
		SmartPtr<std::remove_pointer_t<XIM>, ::XCloseIM> _input_method;
	};
}
