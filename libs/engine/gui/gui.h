//
// Copyright 2018 Sergei Blagodarin
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

#include "actions.h"
#include "cursor.h"
#include "key_lookup.h"

#include <functional>
#include <map>
#include <memory>
#include <unordered_map>

namespace Yttrium
{
	class Canvas;
	class Font;
	class Gui;
	class GuiScreen;
	class MusicReader;
	class RectF;
	class RenderPass;
	class ResourceLoader;
	class ScriptContext;
	class Storage;
	class Texture2D;
	class Translation;
	class Vector2;

	class GuiPrivate
	{
	public:
		GuiPrivate(ResourceLoader&, ScriptContext&);
		~GuiPrivate();

		GuiScreen& add_screen(std::string_view name, bool is_transparent, bool is_root);
		void draw_custom_cursor(RenderPass& pass, const Vector2& point) const
		{
			if (_on_custom_cursor)
				_on_custom_cursor(pass, point);
		}
		std::shared_ptr<const Font> font(const std::string&) const;
		std::shared_ptr<MusicReader> music(const std::string&) const;
		void on_canvas_draw(RenderPass&, const std::string& name, const RectF&) const;
		void on_canvas_mouse_move(const std::string& name, const RectF&, const Vector2&);
		bool on_canvas_mouse_press(const std::string& name, const RectF&, Key, const Vector2&);
		bool pop_screen();
		bool pop_screens_until(const std::string& name);
		bool push_screen(const std::string& name);
		void quit() const
		{
			if (_on_quit)
				_on_quit();
		}
		ResourceLoader& resource_loader() const { return _resource_loader; }
		ScriptContext& script_context() const { return _script_context; }
		void set_default_cursor(GuiCursor, std::string_view texture = {});
		void set_font(const std::string& name, std::string_view path);
		void set_icon_path(std::string_view path) { _icon_path = path; }
		void set_music(std::string_view name, std::string_view file, int start, int end, int loop);
		void set_on_key(std::string_view key, GuiActions&& on_press, GuiActions&& on_release) { _on_key.insert_or_assign(lookup_key(key), std::make_pair(std::move(on_press), std::move(on_release))); }
		void set_title(std::string_view title) { _title = title; }
		void set_translation(std::string_view);
		std::string translate(std::string_view) const;

	private:
		void enter_screen(GuiScreen&);
		void leave_screen();

	private:
		ResourceLoader& _resource_loader;
		ScriptContext& _script_context;
		std::string _title;
		std::string _icon_path;
		std::unordered_map<std::string, std::shared_ptr<MusicReader>> _music; // TODO: Make temporary (it is not used after GUI is loaded).
		std::unordered_map<std::string, std::shared_ptr<const Font>> _fonts;
		std::unordered_map<std::string_view, std::unique_ptr<GuiScreen>> _screens;
		GuiScreen* _root_screen = nullptr;
		std::vector<GuiScreen*> _screen_stack;
		std::shared_ptr<const Translation> _translation;
		std::map<Key, std::pair<GuiActions, GuiActions>> _on_key;
		GuiCursor _default_cursor = GuiCursor::None;
		std::shared_ptr<const Texture2D> _default_cursor_texture;
		std::unordered_map<std::string, Canvas*> _canvases;
		std::function<void(RenderPass&, const Vector2&)> _on_custom_cursor;
		std::function<void(const std::shared_ptr<MusicReader>&)> _on_music;
		std::function<void()> _on_quit;

		friend Gui;
	};
}
