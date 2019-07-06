//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include <yttrium/api.h>

#include <chrono>
#include <functional>
#include <memory>
#include <string>

namespace Yttrium
{
	class AudioManager;
	enum class Key;
	class KeyEvent;
	class RectF;
	class RenderPass;
	class ResourceLoader;
	class ScriptContext;
	class Vector2;

	class Y_ENGINE_API Canvas
	{
	public:
		virtual ~Canvas() = default;
		virtual void on_draw(RenderPass&, const RectF&, std::chrono::milliseconds) {}
		virtual void on_mouse_move(const RectF&, const Vector2&) {}
		virtual bool on_mouse_press(const RectF&, Key, const Vector2&) { return false; }
	};

	class Y_ENGINE_API Gui
	{
	public:
		Gui(std::string_view name, ResourceLoader&, ScriptContext&, const std::shared_ptr<AudioManager>& = {});
		~Gui();

		void bind_canvas(const std::string& name, Canvas&);
		void draw(RenderPass&, const Vector2& cursor) const;
		const std::string& icon_path() const;
		void notify(const std::string& event);
		void on_custom_cursor(const std::function<void(RenderPass&, const Vector2&)>&);
		void on_quit(const std::function<void()>&);
		bool process_key_event(const KeyEvent&);
		bool process_text_input(std::string_view text);
		void start();
		const std::string& title() const;

	private:
		const std::unique_ptr<class GuiPrivate> _private;
	};
}