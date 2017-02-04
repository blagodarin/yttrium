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
#include "layer.h"

#include <algorithm>
#include <cassert>

namespace Yttrium
{
	GuiPrivate::GuiPrivate(ResourceLoader& resource_loader, ScriptContext& script_context, Allocator& allocator)
		: _resource_loader(resource_loader)
		, _script_context(script_context)
		, _allocator(allocator)
		, _layers(_allocator)
	{
	}

	GuiPrivate::~GuiPrivate() = default;

	GuiLayer& GuiPrivate::add_layer(const StaticString& name, bool is_transparent, bool is_root)
	{
		if (!is_root && name.is_empty())
			throw GuiDataError("Non-root layer must have a name"_s);
		if (_layers.find(String(name, ByReference())) != _layers.end())
			throw GuiDataError("Duplicate layer name \""_s, name, "\""_s);
		auto&& layer = make_unique<GuiLayer>(_allocator, *this, name, is_transparent);
		if (is_root)
		{
			if (_root_layer)
				throw GuiDataError("\""_s, name, "\" can't be the root layer, \""_s, _root_layer->name(), "\" is the root layer"_s);
			_root_layer = layer.get();
		}
		return *_layers.emplace(name, std::move(layer)).first->second;
	}

	void GuiPrivate::set_translation(const StaticString& path)
	{
		if (_translation)
			throw GuiDataError("Only one translation is allowed"_s);
		_translation = _resource_loader.load_translation(path);
		if (!_translation)
			throw GuiDataError("Bad translation \""_s, path, "\""_s);
	}

	const GuiPrivate::FontDesc* GuiPrivate::font(const StaticString& name) const
	{
		const auto i = _fonts.find(String(name, ByReference()));
		return i != _fonts.end() ? &i->second : nullptr;
	}

	bool GuiPrivate::pop_layer()
	{
		if (_layer_stack.size() <= 1)
			return false;
		leave_layer();
		return true;
	}

	bool GuiPrivate::pop_layers_until(const StaticString& name)
	{
		const auto end = std::find_if(_layer_stack.rbegin(), _layer_stack.rend(), [&name](GuiLayer* layer){ return layer->name() == name; });
		if (end == _layer_stack.rend())
			return false;
		for (auto n = std::distance(_layer_stack.rbegin(), end); n > 0; --n)
			leave_layer();
		return true;
	}

	bool GuiPrivate::push_layer(const StaticString& name)
	{
		const auto i = _layers.find(String(name, ByReference()));
		if (i == _layers.end())
			return false;
		enter_layer(i->second.get());
		return true;
	}

	void GuiPrivate::set_font(const StaticString& name, const StaticString& font_source, const StaticString& texture_name)
	{
		auto texture = _resource_loader.load_texture_2d(texture_name);
		assert(texture);

		auto font = _resource_loader.load_texture_font(font_source);
		assert(font);

		if (!Rect(texture->size()).contains(font->rect()))
			throw GuiDataError("Can't use font \""_s, font_source, "\" with texture \""_s, texture_name, "\""_s);

		_fonts[String(name, &_allocator)] = FontDesc(std::move(font), std::move(texture));
	}

	String GuiPrivate::translate(const StaticString& source) const
	{
		return _translation ? _translation->translate(source) : String(source, &_allocator);
	}

	void GuiPrivate::enter_layer(GuiLayer* layer)
	{
		_layer_stack.emplace_back(layer);
		if (layer->has_music() && _on_music)
			_on_music(layer->music());
		layer->handle_enter();
	}

	void GuiPrivate::leave_layer()
	{
		const auto layer = _layer_stack.back();
		layer->handle_return();
		if (layer->has_music() && _on_music)
		{
			const auto i = std::find_if(std::next(_layer_stack.rbegin()), _layer_stack.rend(), [](GuiLayer* layer){ return layer->has_music(); });
			_on_music(i != _layer_stack.rend() ? (*i)->music() : nullptr);
		}
		_layer_stack.pop_back();
	}

	Gui::Gui(ResourceLoader& resource_loader, ScriptContext& script_context, const StaticString& name, Allocator& allocator)
		: _private(std::make_unique<GuiPrivate>(resource_loader, script_context, allocator))
	{
		GuiIonLoader(*_private).load(name);
		if (!_private->_root_layer)
			throw GuiDataError("(gui) No root layer has been added"_s);
	}

	Gui::~Gui() = default;

	void Gui::notify(const StaticString& event)
	{
		if (!_private->_layer_stack.empty())
			_private->_layer_stack.back()->handle_event(event.to_std());
	}

	void Gui::on_canvas(const std::function<void(Renderer&, const StaticString&, const RectF&)>& callback)
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
		if (!_private->_layer_stack.empty() && _private->_layer_stack.back()->handle_key(event))
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
		if (_private->_layer_stack.empty())
			return;
		const auto top_layer = std::prev(_private->_layer_stack.end());
		{
			auto layer = top_layer;
			while (layer != _private->_layer_stack.begin() && (*layer)->is_transparent())
				--layer;
			while (layer != top_layer)
				(*layer++)->render(renderer, nullptr);
		}
		(*top_layer)->render(renderer, &cursor);
	}

	void Gui::start()
	{
		assert(_private->_layer_stack.empty()); // TODO: Remove explicit 'start()'.
		_private->enter_layer(_private->_root_layer);
	}
}
