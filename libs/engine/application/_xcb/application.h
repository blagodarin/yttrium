#pragma once

#include "../../../core/utils/memory.h"

#include <xcb/xcb.h>

namespace Yttrium
{
	class NativeApplication
	{
	public:
		NativeApplication();

		xcb_connection_t* connection() const noexcept { return _connection.get(); }
		xcb_screen_t* screen() const noexcept { return _screen; }

	private:
		UniquePtr<xcb_connection_t, ::xcb_disconnect> _connection;
		xcb_screen_t* _screen = nullptr;
	};
}
