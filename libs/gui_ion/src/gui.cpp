// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/gui/ion_gui.h>

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
		explicit Increment(T& value) noexcept
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

namespace Yt
{
	IonGuiPrivate::IonGuiPrivate(ScriptContext& script_context, const std::shared_ptr<AudioManager>& audio_manager) noexcept
		: _script_context{ script_context }
		, _audio_manager{ audio_manager }
	{
	}

	IonGuiPrivate::~IonGuiPrivate() = default;

	GuiScreen& IonGuiPrivate::add_screen(std::string_view name, bool is_root)
	{
		if (!is_root && name.empty())
			throw GuiDataError{ "Non-root screen must have a name" };
		if (_screens.find(name) != _screens.end())
			throw GuiDataError{ "Duplicate screen name \"", name, "\"" };
		auto screen = std::make_unique<GuiScreen>(*this, name);
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

	void IonGuiPrivate::add_startup_command(std::string_view name, std::vector<std::string>&& args)
	{
		_startup_commands.emplace_back(name, std::move(args));
	}

	std::shared_ptr<Sound> IonGuiPrivate::load_sound(ResourceLoader& loader, std::string_view name)
	{
		if (!_audio_manager)
			return {};
		// cppcheck-suppress stlIfFind
		if (const auto i = _sounds.find(name); i != _sounds.end())
			return i->second;
		return _sounds.emplace(name, _audio_manager->create_sound(loader.open(name))).first->second;
	}

	void IonGuiPrivate::on_canvas_draw(RenderPass& pass, const std::string& name, const RectF& rect) const
	{
		const auto i = _canvases.find(name);
		if (i != _canvases.end())
			i->second->on_draw(pass, rect, _screen_stack.back()->time_since_enter());
	}

	void IonGuiPrivate::on_canvas_mouse_move(const std::string& name, const RectF& rect, const Vector2& cursor)
	{
		const auto i = _canvases.find(name);
		if (i != _canvases.end())
			i->second->on_mouse_move(rect, cursor);
	}

	bool IonGuiPrivate::on_canvas_mouse_press(const std::string& name, const RectF& rect, Key key, const Vector2& cursor)
	{
		const auto i = _canvases.find(name);
		return i != _canvases.end() && i->second->on_mouse_press(rect, key, cursor);
	}

	bool IonGuiPrivate::pop_screen()
	{
		if (_screen_stack.size() <= 1)
			return false;
		leave_screen();
		return true;
	}

	bool IonGuiPrivate::pop_screens_until(const std::string& name)
	{
		const auto end = std::find_if(_screen_stack.rbegin(), _screen_stack.rend(), [&name](GuiScreen* screen) { return screen->name() == name; });
		if (end == _screen_stack.rend())
			return false;
		for (auto n = std::distance(_screen_stack.rbegin(), end); n > 0; --n)
			leave_screen();
		return true;
	}

	bool IonGuiPrivate::push_screen(const std::string& name)
	{
		const auto i = _screens.find(name);
		if (i == _screens.end())
			return false;
		enter_screen(*i->second);
		return true;
	}

	void IonGuiPrivate::set_default_cursor(GuiCursor cursor, const std::shared_ptr<const Texture2D>& texture)
	{
		_default_cursor = cursor;
		_default_cursor_texture = texture;
	}

	void IonGuiPrivate::enter_screen(GuiScreen& screen)
	{
		Increment recursion{ _screen_recursion };
		_screen_stack.emplace_back(&screen);
		screen.handle_enter();
		if (!recursion.decrement())
			update_music();
	}

	void IonGuiPrivate::leave_screen()
	{
		Increment recursion{ _screen_recursion };
		const auto screen = _screen_stack.back();
		_screen_stack.pop_back();
		screen->handle_return();
		if (!recursion.decrement())
			update_music();
	}

	void IonGuiPrivate::update_music()
	{
		if (!_audio_manager)
			return;
		const auto i = std::find_if(_screen_stack.rbegin(), _screen_stack.rend(), [](GuiScreen* s) { return s->has_music(); });
		auto music = i != _screen_stack.rend() ? (*i)->music() : nullptr;
		if (music == _current_music)
			return;
		_current_music = std::move(music);
		_audio_manager->play_music(_current_music);
	}

	IonGui::IonGui(std::string_view name, ResourceLoader& resource_loader, ScriptContext& script_context, const std::shared_ptr<AudioManager>& audio_manager)
		: _private{ std::make_unique<IonGuiPrivate>(script_context, audio_manager) }
	{
		load_ion_gui(*_private, resource_loader, name);
		if (!_private->_root_screen)
			throw GuiDataError{ "(gui) No root screen has been added" };
	}

	IonGui::~IonGui() = default;

	void IonGui::bind_canvas(const std::string& name, IonGuiCanvas& canvas)
	{
		_private->_canvases[name] = &canvas;
	}

	void IonGui::draw(RenderPass& pass, const Vector2& cursor) const
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

	const std::string& IonGui::icon_path() const
	{
		return _private->_icon_path;
	}

	void IonGui::notify(const std::string& event)
	{
		if (!_private->_screen_stack.empty())
			_private->_screen_stack.back()->handle_event(event);
	}

	void IonGui::on_custom_cursor(const std::function<void(RenderPass&, const Vector2&)>& callback)
	{
		_private->_on_custom_cursor = callback;
	}

	void IonGui::on_quit(const std::function<void()>& callback)
	{
		_private->_on_quit = callback;
	}

	bool IonGui::process_key_event(const KeyEvent& event)
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

	bool IonGui::process_text_input(std::string_view text)
	{
		return !_private->_screen_stack.empty() && _private->_screen_stack.back()->handle_text(text);
	}

	void IonGui::start()
	{
		for (const auto& command : _private->_startup_commands)
			_private->_script_context.call(command.first, command.second);
		assert(_private->_screen_stack.empty());         // TODO: Remove explicit 'start()'.
		_private->enter_screen(*_private->_root_screen); // TODO: Move it into startup commands.
	}

	const std::string& IonGui::title() const
	{
		return _private->_title;
	}
}