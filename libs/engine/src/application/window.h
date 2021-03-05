// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../renderer/builtin.h"
#include "../renderer/pass.h"
#include "../renderer/renderer.h"
#include "application.h"
#include "window_backend.h"
#include "window_callbacks.h"

#include <functional>

namespace Yt
{
	class RenderPass;
	struct RenderReport;
	class Window;

	class WindowPrivate final : private WindowBackendCallbacks
	{
	public:
		explicit WindowPrivate(Application&);

		bool process_events() { return _backend.process_events(); }
		void render(RenderReport& next_report, const RenderReport& last_report);
		void update();

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
		WindowBackend _backend{ *this };
		RendererImpl _renderer{ _backend };
		RenderBuiltin _renderer_builtin{ *_renderer._backend };
		RenderPassData _render_pass_data;
		bool _is_active = false;
		Point _cursor;
		bool _is_cursor_locked = false;
		Size _size;
		bool _take_screenshot = false;
		std::function<void(int, int)> _on_cursor_moved;
		std::function<void(const KeyEvent&)> _on_key_event;
		std::function<void(RenderPass&, const Vector2&, const RenderReport&)> _on_render;
		std::function<void(Image&&)> _on_screenshot;
		std::function<void(std::string_view)> _on_text_input;

		friend Window;
	};
}
