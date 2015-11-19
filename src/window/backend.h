#ifndef _src_window_backend_h_
#define _src_window_backend_h_

namespace Yttrium
{
	class WindowBackendCallbacks
	{
	public:

		virtual void on_focus_event(bool is_focused) = 0;
		virtual void on_key_event(Key key, bool is_pressed) = 0;
	};
}

#endif
