// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <primal/pointer.hpp>

#include <type_traits>

#include <X11/Xlib.h>

namespace Yt
{
	class NativeApplication
	{
	public:
		NativeApplication();

		::Display* display() const noexcept { return _display; }
		XIM input_method() const noexcept { return _input_method; }
		int screen() const noexcept { return _screen; }

	private:
		primal::CPtr<::Display, ::XCloseDisplay> _display;
		int _screen = 0;
		primal::CPtr<std::remove_pointer_t<XIM>, ::XCloseIM> _input_method;
	};
}
