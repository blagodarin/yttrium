// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/geometry/point.h>
#include <yttrium/geometry/size.h>
#include "application.h"
#include "window_backend.h"
#include "window_callbacks.h"

namespace Yt
{
	class RenderPass;
	class Window;
	class WindowCallbacks;

	class WindowPrivate final : private WindowBackendCallbacks
	{
	public:
		explicit WindowPrivate(Application&, Window&);

		bool process_events(EventCallbacks&);
		void update(EventCallbacks&);

	private:
		void on_focus_event(bool is_focused) override;
		void on_key_event(Key, bool pressed, bool autorepeat, Flags<KeyEvent::Modifier>) override;
		void on_resize_event(const Size&) override;
		void on_text_input(std::string_view) override;

	private:
		void lock_cursor(bool);
		void set_active(bool);

	private:
		const ApplicationStub _application;
		Window& _window;
		EventCallbacks* _callbacks = nullptr;
		WindowBackend _backend{ *this };
		bool _is_active = false;
		Point _cursor;
		bool _is_cursor_locked = false;
		Size _size;

		friend Window;
	};
}
