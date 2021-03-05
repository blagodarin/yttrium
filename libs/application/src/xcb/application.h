// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <primal/pointer.hpp>

#include <xcb/xcb.h>

namespace Yt
{
	class NativeApplication
	{
	public:
		NativeApplication();

		xcb_connection_t* connection() const noexcept { return _connection; }
		xcb_screen_t* screen() const noexcept { return _screen; }

	private:
		primal::CPtr<xcb_connection_t, ::xcb_disconnect> _connection;
		xcb_screen_t* _screen = nullptr;
	};
}
