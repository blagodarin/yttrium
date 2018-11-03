#pragma once

#include <yttrium/key.h>

#include <optional>
#include <string_view>

namespace Yttrium
{
	class Size;

	class WindowBackendCallbacks
	{
	public:
		virtual ~WindowBackendCallbacks() = default;
		virtual void on_focus_event(bool is_focused) = 0;
		virtual void on_key_event(Key, bool is_pressed, const std::optional<Flags<KeyEvent::Modifier>>&) = 0;
		virtual void on_resize_event(const Size&) = 0;
		virtual void on_text_event(std::string_view) = 0;
	};
}
