// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "application.h"

#include <yttrium/base/exceptions.h>

namespace Yt
{
	NativeApplication::NativeApplication()
	{
		int preferred_screen = 0;
		_connection.reset(::xcb_connect(nullptr, &preferred_screen));
		if (::xcb_connection_has_error(_connection))
			throw InitializationError{ "xcb_connect failed" };
		auto screen_iterator = ::xcb_setup_roots_iterator(::xcb_get_setup(_connection));
		for (; preferred_screen > 0; --preferred_screen)
			::xcb_screen_next(&screen_iterator);
		_screen = screen_iterator.data;
	}
}
