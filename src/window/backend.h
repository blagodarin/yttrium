#ifndef _src_window_backend_h_
#define _src_window_backend_h_

#include <yttrium/key.h>

namespace Yttrium
{
	class Size;

	class WindowBackendCallbacks
	{
	public:
		virtual void on_focus_event(bool is_focused) = 0;
		virtual void on_key_event(Key key, bool is_pressed) = 0;
		virtual void on_resize_event(const Size&) = 0;
	};
}

#endif
