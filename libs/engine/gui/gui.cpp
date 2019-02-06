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

#include <yttrium/gui/gui.h>

#include <yttrium/audio/music_reader.h>
#include <yttrium/exceptions.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/resource_loader.h>
#include <yttrium/script/context.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/storage.h>
#include "gui.h"
#include "ion/loader.h"
#include "screen.h"

#include <algorithm>
#include <cassert>

namespace
{
	template <typename T>
	class Increment
	{
	public:
		Increment(T& value) noexcept
			: _value{ value }
		{
			++_value;
		}

		~Increment() noexcept
		{
			if (!_decremented)
				--_value;
		}

		T decrement() noexcept
		{
			--_value;
			_decremented = true;
			return _value;
		}

	private:
		T& _value;
		bool _decremented = false;
	};
}

namespace Yttrium
{
	GuiPrivate::GuiPrivate(ScriptContext& script_context) noexcept
		: _script_context{ script_context }
	{
	}

	GuiPrivate::~GuiPrivate() = default;

	GuiScreen& GuiPrivate::add_screen(std::string_view name, bool is_transparent, bool is_root)
	{
		if (!is_root && name.empty())
			throw GuiDataError{ "Non-root screen must have a name" };
		if (_screens.find(name) != _screens.end())
			throw GuiDataError{ "Duplicate screen name \"", name, "\"" };
		auto screen = std::make_unique<GuiScreen>(*this, name, is_transparent);
		if (is_root)
		{
			if (_root_screen)
				throw GuiDataError{ "\"", name, "\" can't be a root screen, \"", _root_screen->name(), "\" is the root screen" };
			_root_screen = screen.get();
		}
		auto& result = *_screens.emplace(screen->name(), std::move(screen)).first->second;
		result.set_cursor(_default_cursor, _default_cursor_texture);
		return result;
	}

	void GuiPrivate::on_canvas_draw(RenderPass& pass, const std::string& name, const RectF& rect) const
	{
		const auto i = _canvases.find(name);
		if (i != _canvases.end())
			i->second->on_draw(pass, rect, _screen_stack.back()->time_since_enter());
	}

	void GuiPrivate::on_canvas_mouse_move(const std::string& name, const RectF& rect, const Vector2& cursor)
	{
		const auto i = _canvases.find(name);
		if (i != _canvases.end())
			i->second->on_mouse_move(rect, cursor);
	}

	bool GuiPrivate::on_canvas_mouse_press(const std::string& name, const RectF& rect, Key key, const Vector2& cursor)
	{
		const auto i = _canvases.find(name);
		return i != _canvases.end() && i->second->on_mouse_press(rect, key, cursor);
	}

	bool GuiPrivate::pop_screen()
	{
		if (_screen_stack.size() <= 1)
			return false;
		leave_screen();
		return true;
	}

	bool GuiPrivate::pop_screens_until(const std::string& name)
	{
		const auto end = std::find_if(_screen_stack.rbegin(), _screen_stack.rend(), [&name](GuiScreen* screen) { return screen->name() == name; });
		if (end == _screen_stack.rend())
			return false;
		for (auto n = std::distance(_screen_stack.rbegin(), end); n > 0; --n)
			leave_screen();
		return true;
	}

	bool GuiPrivate::push_screen(const std::string& name)
	{
		const auto i = _screens.find(name);
		if (i == _screens.end())
			return false;
		enter_screen(*i->second);
		return true;
	}

	void GuiPrivate::set_default_cursor(GuiCursor cursor, const std::shared_ptr<const Texture2D>& texture)
	{
		_default_cursor = cursor;
		_default_cursor_texture = texture;
	}

	void GuiPrivate::enter_screen(GuiScreen& screen)
	{
		Increment recursion{ _screen_recursion };
		_screen_stack.emplace_back(&screen);
		screen.handle_enter();
		if (!recursion.decrement() && update_music())
			_on_music(_current_music);
	}

	void GuiPrivate::leave_screen()
	{
		Increment recursion{ _screen_recursion };
		const auto screen = _screen_stack.back();
		_screen_stack.pop_back();
		screen->handle_return();
		if (!recursion.decrement() && update_music())
			_on_music(_current_music);
	}

	bool GuiPrivate::update_music() noexcept
	{
		const auto i = std::find_if(_screen_stack.rbegin(), _screen_stack.rend(), [](GuiScreen* s) { return s->has_music(); });
		auto music = i != _screen_stack.rend() ? (*i)->music() : nullptr;
		if (music == _current_music)
			return false;
		_current_music = std::move(music);
		return true;
	}

	Gui::Gui(ResourceLoader& resource_loader, ScriptContext& script_context, std::string_view name)
		: _private(std::make_unique<GuiPrivate>(script_context))
	{
		GuiIonLoader{ *_private, resource_loader }.load(name);
		if (!_private->_root_screen)
			throw GuiDataError{ "(gui) No root screen has been added" };
	}

	Gui::~Gui() = default;

	void Gui::bind_canvas(const std::string& name, Canvas& canvas)
	{
		_private->_canvases[name] = &canvas;
	}

	void Gui::draw(RenderPass& pass, const Vector2& cursor) const
	{
		if (_private->_screen_stack.empty())
			return;
		const auto top_screen = std::prev(_private->_screen_stack.end());
		{
			auto screen = top_screen;
			while (screen != _private->_screen_stack.begin() && (*screen)->is_transparent())
				--screen;
			while (screen != top_screen)
				(*screen++)->draw(pass, nullptr);
		}
		(*top_screen)->draw(pass, &cursor);
	}

	const std::string& Gui::icon_path() const
	{
		return _private->_icon_path;
	}

	void Gui::notify(const std::string& event)
	{
		if (!_private->_screen_stack.empty())
			_private->_screen_stack.back()->handle_event(event);
	}

	void Gui::on_custom_cursor(const std::function<void(RenderPass&, const Vector2&)>& callback)
	{
		_private->_on_custom_cursor = callback;
	}

	void Gui::on_music(const std::function<void(const std::shared_ptr<MusicReader>&)>& callback)
	{
		_private->_on_music = callback;
	}

	void Gui::on_quit(const std::function<void()>& callback)
	{
		_private->_on_quit = callback;
	}

	bool Gui::process_key_event(const KeyEvent& event)
	{
		if (!_private->_screen_stack.empty() && _private->_screen_stack.back()->handle_key(event))
			return true;
		if (!event._autorepeat)
		{
			const auto i = _private->_on_key.find(event._key);
			if (i != _private->_on_key.end())
			{
				(event._pressed ? i->second.first : i->second.second).run(*_private);
				return true;
			}
		}
		return false;
	}

	bool Gui::process_text_input(std::string_view text)
	{
		return !_private->_screen_stack.empty() && _private->_screen_stack.back()->handle_text(text);
	}

	void Gui::start()
	{
		assert(_private->_screen_stack.empty()); // TODO: Remove explicit 'start()'.
		_private->enter_screen(*_private->_root_screen);
	}

	const std::string& Gui::title() const
	{
		return _private->_title;
	}
}
