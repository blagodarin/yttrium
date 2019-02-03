//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include "../renderer/builtin/builtin.h"
#include "../renderer/pass.h"
#include "../renderer/renderer.h"
#include "application.h"
#include "window_backend.h"
#include "window_callbacks.h"

#include <functional>

namespace Yttrium
{
	class RenderPass;
	struct UpdateEvent;
	class Window;

	class WindowPrivate final : private WindowBackendCallbacks
	{
	public:
		explicit WindowPrivate(Application&);

		bool process_events() { return _backend.process_events(); }
		void render(UpdateEvent&);
		void update();

	private:
		void on_focus_event(bool is_focused) override;
		void on_key_event(Key, bool pressed, const std::optional<bool>& autorepeat, Flags<KeyEvent::Modifier>) override;
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
		bool _keys[KeyCount] = {};
		bool _take_screenshot = false;
		std::function<void(int, int)> _on_cursor_moved;
		std::function<void(const KeyEvent&)> _on_key_event;
		std::function<void(RenderPass&, const Vector2&)> _on_render;
		std::function<void(Image&&)> _on_screenshot;
		std::function<void(std::string_view)> _on_text_input;

		friend Window;
	};
}
