// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <seir_base/pointer.hpp>

#include <type_traits>

#include <X11/Xlib.h>
#ifdef None
#	undef None
constexpr long None = 0;
#endif

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
		seir::CPtr<::Display, ::XCloseDisplay> _display;
		int _screen = 0;
		seir::CPtr<std::remove_pointer_t<XIM>, ::XCloseIM> _input_method;
	};
}
