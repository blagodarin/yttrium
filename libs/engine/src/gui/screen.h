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

#include <yttrium/math/rect.h>
#include "actions.h"
#include "cursor.h"
#include "key_lookup.h"
#include "layout.h"

#include <chrono>
#include <map>
#include <unordered_map>

namespace Yt
{
	class AudioReader;
	class GuiLayout;
	class GuiPrivate;
	class RenderPass;
	class Texture2D;
	class Widget;

	class GuiScreen
	{
	public:
		GuiScreen(GuiPrivate&, std::string_view name);
		~GuiScreen();

		GuiLayout& add_layout(GuiLayout::Placement);
		void draw(RenderPass&, const Vector2* cursor);
		void handle_enter();
		bool handle_event(const std::string&) const;
		bool handle_key(const KeyEvent&);
		void handle_return() const { _on_return.run(_gui); }
		bool handle_text(std::string_view);
		bool has_music() const noexcept { return _has_music; }
		bool is_transparent() const noexcept { return _is_transparent; }
		const std::shared_ptr<AudioReader>& music() const { return _music; }
		std::string_view name() const { return _name; }
		void register_widget(Widget&);
		void set_cursor(GuiCursor cursor, const std::shared_ptr<const Texture2D>& texture)
		{
			_cursor = cursor;
			_cursor_texture = texture;
		}
		void set_music(std::shared_ptr<AudioReader>&&);
		void set_on_enter(GuiActions&& actions) { _on_enter = std::move(actions); }
		void set_on_event(std::string_view event, GuiActions&& actions) { _on_event.insert_or_assign(std::string{ event }, std::move(actions)); }
		void set_on_key(std::string_view key, GuiActions&& on_press, GuiActions&& on_release) { _on_key.insert_or_assign(lookup_key(key), std::make_pair(std::move(on_press), std::move(on_release))); }
		void set_on_return(GuiActions&& actions) { _on_return = std::move(actions); }
		void set_transparent(bool transparent) noexcept { _is_transparent = transparent; }
		std::chrono::milliseconds time_since_enter() const noexcept { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _enter_time); }

	private:
		Widget* widget_at(const Vector2&) const;

	private:
		class Activity;

		GuiPrivate& _gui;
		const std::string _name;
		bool _is_transparent = false;
		std::vector<std::unique_ptr<GuiLayout>> _layouts;
		std::vector<Widget*> _widgets;
		const std::unique_ptr<Activity> _activity;
		GuiActions _on_enter;
		std::unordered_map<std::string, GuiActions> _on_event;
		std::map<Key, std::pair<GuiActions, GuiActions>> _on_key;
		GuiActions _on_return;
		GuiCursor _cursor = GuiCursor::None;
		std::shared_ptr<const Texture2D> _cursor_texture;
		std::shared_ptr<AudioReader> _music;
		bool _has_music = false;
		std::chrono::steady_clock::time_point _enter_time;
	};
}
