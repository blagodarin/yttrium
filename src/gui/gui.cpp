#include <yttrium/gui/gui.h>

#include <yttrium/exceptions.h>
#include <yttrium/gui/texture_font.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/resource_loader.h>
#include <yttrium/script/context.h>
#include <yttrium/storage/storage.h>
#include <yttrium/translation.h>
#include "gui.h"
#include "ion/loader.h"
#include "screen.h"

#include <algorithm>
#include <cassert>

namespace Yttrium
{
	GuiPrivate::GuiPrivate(ResourceLoader& resource_loader, ScriptContext& script_context)
		: _resource_loader{resource_loader}
		, _script_context{script_context}
	{
	}

	GuiPrivate::~GuiPrivate() = default;

	GuiScreen& GuiPrivate::add_screen(std::string_view name, bool is_transparent, bool is_root)
	{
		if (!is_root && name.empty())
			throw GuiDataError{"Non-root screen must have a name"};
		if (_screens.find(name) != _screens.end())
			throw GuiDataError{"Duplicate screen name \"", name, "\""};
		auto screen = std::make_unique<GuiScreen>(*this, name, is_transparent);
		if (is_root)
		{
			if (_root_screen)
				throw GuiDataError{"\"", name, "\" can't be a root screen, \"", _root_screen->name(), "\" is the root screen"};
			_root_screen = screen.get();
		}
		auto& result = *_screens.emplace(screen->name(), std::move(screen)).first->second;
		result.set_cursor(_default_cursor, _default_cursor_texture);
		return result;
	}

	void GuiPrivate::set_translation(std::string_view path)
	{
		if (_translation)
			throw GuiDataError{"Only one translation is allowed"};
		_translation = _resource_loader.load_translation(path);
	}

	const GuiPrivate::FontDesc* GuiPrivate::font(const std::string& name) const
	{
		const auto i = _fonts.find(name);
		return i != _fonts.end() ? &i->second : nullptr;
	}

	void GuiPrivate::on_canvas_draw(RenderContext& context, const std::string& name, const RectF& rect) const
	{
		const auto i = _canvases.find(name);
		if (i != _canvases.end())
			i->second->on_draw(context, rect);
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
		const auto end = std::find_if(_screen_stack.rbegin(), _screen_stack.rend(), [&name](GuiScreen* screen){ return screen->name() == name; });
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

	void GuiPrivate::set_default_cursor(GuiCursor cursor, std::string_view texture)
	{
		_default_cursor = cursor;
		if (_default_cursor == GuiCursor::Texture)
			_default_cursor_texture = _resource_loader.load_texture_2d(texture);
	}

	void GuiPrivate::set_font(const std::string& name, std::string_view font_source, std::string_view texture_name)
	{
		auto texture = _resource_loader.load_texture_2d(texture_name);
		assert(texture);

		auto texture_font = _resource_loader.load_texture_font(font_source);
		assert(texture_font);

		if (!Rect(texture->size()).contains(texture_font->rect()))
			throw GuiDataError{"Can't use font \"", font_source, "\" with texture \"", texture_name, "\""};

		auto& font = _fonts[name];
		font.font = std::move(texture_font);
		font.texture = std::move(texture);
	}

	std::string GuiPrivate::translate(std::string_view source) const
	{
		return _translation ? _translation->translate(source) : std::string{source};
	}

	void GuiPrivate::enter_screen(GuiScreen& screen)
	{
		_screen_stack.emplace_back(&screen);
		if (screen.music() && _on_music)
			_on_music(screen.music());
		screen.handle_enter();
	}

	void GuiPrivate::leave_screen()
	{
		const auto screen = _screen_stack.back();
		screen->handle_return();
		if (screen->music() && _on_music)
		{
			const auto i = std::find_if(std::next(_screen_stack.rbegin()), _screen_stack.rend(), [](GuiScreen* s){ return s->music(); });
			_on_music(i != _screen_stack.rend() ? (*i)->music() : nullptr);
		}
		_screen_stack.pop_back();
	}

	Gui::Gui(ResourceLoader& resource_loader, ScriptContext& script_context, std::string_view name)
		: _private(std::make_unique<GuiPrivate>(resource_loader, script_context))
	{
		GuiIonLoader(*_private).load(name);
		if (!_private->_root_screen)
			throw GuiDataError{"(gui) No root screen has been added"};
	}

	Gui::~Gui() = default;

	void Gui::bind_canvas(const std::string& name, Canvas& canvas)
	{
		_private->_canvases[name] = &canvas;
	}

	void Gui::draw(RenderContext& context, const Vector2& cursor) const
	{
		if (_private->_screen_stack.empty())
			return;
		const auto top_screen = std::prev(_private->_screen_stack.end());
		{
			auto screen = top_screen;
			while (screen != _private->_screen_stack.begin() && (*screen)->is_transparent())
				--screen;
			while (screen != top_screen)
				(*screen++)->draw(context, nullptr);
		}
		(*top_screen)->draw(context, &cursor);
	}

	void Gui::notify(const std::string& event)
	{
		if (!_private->_screen_stack.empty())
			_private->_screen_stack.back()->handle_event(event);
	}

	void Gui::on_custom_cursor(const std::function<void(RenderContext&, const Vector2&)>& callback)
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
		if (!event.autorepeat)
		{
			const auto i = _private->_on_key.find(event.key);
			if (i != _private->_on_key.end())
			{
				(event.pressed ? i->second.first : i->second.second).run(*_private);
				return true;
			}
		}
		return false;
	}

	void Gui::start()
	{
		assert(_private->_screen_stack.empty()); // TODO: Remove explicit 'start()'.
		_private->enter_screen(*_private->_root_screen);
	}
}
