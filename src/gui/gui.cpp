#include <yttrium/gui/gui.h>

#include <yttrium/exceptions.h>
#include <yttrium/gui/texture_font.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/resources/translation.h>
#include <yttrium/script/context.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/storage.h>
#include "gui.h"
#include "ion/loader.h"
#include "screen.h"

#include <algorithm>
#include <cassert>

namespace Yttrium
{
	GuiPrivate::GuiPrivate(ResourceLoader& resource_loader, ScriptContext& script_context, Allocator& allocator)
		: _resource_loader(resource_loader)
		, _script_context(script_context)
		, _allocator(allocator)
	{
	}

	GuiPrivate::~GuiPrivate() = default;

	GuiScreen& GuiPrivate::add_screen(const std::string& name, bool is_transparent, bool is_root)
	{
		if (!is_root && name.empty())
			throw GuiDataError("Non-root screen must have a name"_s);
		if (_screens.find(name) != _screens.end())
			throw GuiDataError("Duplicate screen name \""_s, name, "\""_s);
		auto screen = std::make_unique<GuiScreen>(*this, name, is_transparent);
		if (is_root)
		{
			if (_root_screen)
				throw GuiDataError("\""_s, name, "\" can't be a root screen, \""_s, _root_screen->name(), "\" is the root screen"_s);
			_root_screen = screen.get();
		}
		auto& result = *_screens.emplace(screen->name(), std::move(screen)).first->second;
		result.set_cursor(_default_cursor, _default_cursor_texture);
		return result;
	}

	void GuiPrivate::set_translation(const StaticString& path)
	{
		if (_translation)
			throw GuiDataError("Only one translation is allowed"_s);
		_translation = _resource_loader.load_translation(path);
		if (!_translation)
			throw GuiDataError("Bad translation \""_s, path, "\""_s);
	}

	const GuiPrivate::FontDesc* GuiPrivate::font(const std::string& name) const
	{
		const auto i = _fonts.find(name);
		return i != _fonts.end() ? &i->second : nullptr;
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

	void GuiPrivate::set_default_cursor(GuiCursor cursor, const StaticString& texture)
	{
		_default_cursor = cursor;
		if (_default_cursor == GuiCursor::Texture)
			_default_cursor_texture = _resource_loader.load_texture_2d(texture);
	}

	void GuiPrivate::set_font(const std::string& name, const StaticString& font_source, const StaticString& texture_name)
	{
		auto texture = _resource_loader.load_texture_2d(texture_name);
		assert(texture);

		auto texture_font = _resource_loader.load_texture_font(font_source);
		assert(texture_font);

		if (!Rect(texture->size()).contains(texture_font->rect()))
			throw GuiDataError("Can't use font \""_s, font_source, "\" with texture \""_s, texture_name, "\""_s);

		auto& font = _fonts[name];
		font.font = std::move(texture_font);
		font.texture = std::move(texture);
	}

	std::string GuiPrivate::translate(const StaticString& source) const
	{
		return _translation ? _translation->translate(source) : source.to_std();
	}

	void GuiPrivate::enter_screen(GuiScreen& screen)
	{
		_screen_stack.emplace_back(&screen);
		if (screen.has_music() && _on_music)
			_on_music(screen.music());
		screen.handle_enter();
	}

	void GuiPrivate::leave_screen()
	{
		const auto screen = _screen_stack.back();
		screen->handle_return();
		if (screen->has_music() && _on_music)
		{
			const auto i = std::find_if(std::next(_screen_stack.rbegin()), _screen_stack.rend(), [](GuiScreen* screen){ return screen->has_music(); });
			_on_music(i != _screen_stack.rend() ? (*i)->music() : nullptr);
		}
		_screen_stack.pop_back();
	}

	Gui::Gui(ResourceLoader& resource_loader, ScriptContext& script_context, const StaticString& name, Allocator& allocator)
		: _private(std::make_unique<GuiPrivate>(resource_loader, script_context, allocator))
	{
		GuiIonLoader(*_private).load(name);
		if (!_private->_root_screen)
			throw GuiDataError("(gui) No root screen has been added"_s);
	}

	Gui::~Gui() = default;

	void Gui::notify(const StaticString& event)
	{
		if (!_private->_screen_stack.empty())
			_private->_screen_stack.back()->handle_event(event.to_std());
	}

	void Gui::on_canvas(const std::function<void(Renderer&, const std::string&, const RectF&)>& callback)
	{
		_private->_on_canvas = callback;
	}

	void Gui::on_custom_cursor(const std::function<void(Renderer&, const PointF&)>& callback)
	{
		_private->_on_custom_cursor = callback;
	}

	void Gui::on_music(const std::function<void(const ResourcePtr<const Music>&)>& callback)
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

	void Gui::render(Renderer& renderer, const PointF& cursor) const
	{
		if (_private->_screen_stack.empty())
			return;
		const auto top_screen = std::prev(_private->_screen_stack.end());
		{
			auto screen = top_screen;
			while (screen != _private->_screen_stack.begin() && (*screen)->is_transparent())
				--screen;
			while (screen != top_screen)
				(*screen++)->render(renderer, nullptr);
		}
		(*top_screen)->render(renderer, &cursor);
	}

	void Gui::start()
	{
		assert(_private->_screen_stack.empty()); // TODO: Remove explicit 'start()'.
		_private->enter_screen(*_private->_root_screen);
	}
}
