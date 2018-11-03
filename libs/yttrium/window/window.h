#pragma once

#include <functional>

#include "../platform/window.h"
#include "../renderer/builtin/builtin.h"
#include "../renderer/renderer.h"
#include "backend.h"
#include "event_loop.h"

namespace Yttrium
{
	class RenderPass;
	struct UpdateEvent;

	class WindowPrivate final : private WindowBackendCallbacks
	{
	public:
		WindowPrivate(EventLoop& event_loop, std::string_view name);

		void run();

	private:
		void on_focus_event(bool is_focused) override;
		void on_key_event(Key, bool is_pressed, const std::optional<Flags<KeyEvent::Modifier>>&) override;
		void on_resize_event(const Size&) override;
		void on_text_event(std::string_view) override;

	private:
		void lock_cursor(bool);
		bool process_events();
		void set_active(bool);

	private:
		const EventLoopStub _event_loop;
		const std::string _name;
		WindowBackend _backend{_name, *this};
		RendererImpl _renderer{_backend};
		RenderBuiltin _renderer_builtin{*_renderer._backend};
		bool _is_active = false;
		Point _cursor;
		bool _is_cursor_locked = false;
		Size _size;
		bool _fullscreen = false;
		bool _keys[KeyCount] = {};
		bool _take_screenshot = false;
		std::function<void(int, int)> _on_cursor_moved;
		std::function<void(const KeyEvent&)> _on_key_event;
		std::function<void(RenderPass&, const Vector2&)> _on_render;
		std::function<void(Image&&)> _on_screenshot;
		std::function<void(std::string_view)> _on_text_event;
		std::function<void(const UpdateEvent&)> _on_update;

		friend Window;
	};
}
