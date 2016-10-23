#include <yttrium/gui.h>

#include <yttrium/exceptions.h>
#include <yttrium/io/reader.h>
#include <yttrium/io/storage.h>
#include <yttrium/log.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/resources/texture_font.h>
#include <yttrium/resources/translation.h>
#include <yttrium/script/context.h>
#include "exceptions.h"
#include "gui.h"
#include "ion/loader.h"
#include "layer.h"

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
			throw GuiError(_allocator) << "Non-root layer must have a name"_s;
		auto&& layer = make_unique<GuiLayer>(_allocator, *this, name, is_transparent);
		const String& layer_name = layer->name(); // To avoid extra allocations.
		if (_layers.find(layer_name) != _layers.end())
			throw GuiError(_allocator) << "Duplicate layer name \""_s << layer_name << "\""_s;
		if (is_root)
		{
			if (!_layer_stack.empty())
				throw GuiError(_allocator) << "\""_s << layer_name << "\" can't be the root layer, \""_s << _layer_stack.front()->name() << "\" is the root layer"_s;
			_layer_stack.push_back(layer.get());
		}
		return *_layers.emplace(layer_name, std::move(layer)).first->second;
	}

	void GuiPrivate::set_translation(const StaticString& path)
	{
		if (_translation)
			throw GuiError(_allocator) << "Only one translation is allowed"_s;
		_translation = _resource_loader.load_translation(path);
		if (!_translation)
			throw GuiError(_allocator) << "Bad translation \""_s << path << "\""_s;
	}

	void GuiPrivate::clear()
	{
		_fonts.clear();
		_layers.clear();
		_layer_stack.clear();
	}

	const GuiPrivate::FontDesc* GuiPrivate::font(const StaticString& name) const
	{
		const auto i = _fonts.find(String(name, ByReference()));
		return i != _fonts.end() ? &i->second : nullptr;
	}

	void GuiPrivate::render_canvas(Renderer& renderer, const StaticString& name, const RectF& rect) const
	{
		const auto i = _canvas_handlers.find(String(name, ByReference()));
		if (i != _canvas_handlers.end())
			i->second(renderer, rect);
	}

	void GuiPrivate::set_font(const StaticString& name, const StaticString& font_source, const StaticString& texture_name)
	{
		auto texture = _resource_loader.load_texture_2d(texture_name, true);
		if (!texture)
		{
			Log() << "Can't load \""_s << texture_name << "\""_s; // TODO: Move to ResourceLoader.
			return;
		}

		auto font = _resource_loader.load_texture_font(font_source);
		if (!font)
			return;

		if (!Rect(texture->size()).contains(font->rect()))
		{
			Log() << "Can't use \""_s << font_source << "\" with \""_s << texture_name << "\""_s;
			return;
		}

		_fonts[String(name, &_allocator)] = FontDesc(std::move(font), std::move(texture));
	}

	String GuiPrivate::translate(const StaticString& source) const
	{
		return _translation ? _translation->translate(source) : String(source, &_allocator);
	}

	Gui::Gui(ResourceLoader& resource_loader, ScriptContext& script_context, Allocator& allocator)
		: _private(make_unique<GuiPrivate>(allocator, resource_loader, script_context, allocator))
	{
	}

	void Gui::clear()
	{
		_private->clear();
	}

	bool Gui::has_layer(const StaticString& name) const
	{
		return _private->_layers.find(String(name, ByReference())) != _private->_layers.end();
	}

	bool Gui::load(const StaticString& filename)
	{
		clear();
		GuiIonLoader loader(*_private);
		if (!loader.load(filename))
			return false;
		if (_private->_layer_stack.empty())
		{
			Log() << "(gui) No root layer has been added"_s;
			clear();
			return false;
		}
		return true;
	}

	bool Gui::pop_layers(size_t count)
	{
		for (auto n = min(count, _private->_layer_stack.size()); n > 0; --n)
		{
			_private->_layer_stack.back()->run_action(GuiLayer::Action::Pop, _private->_script_context);
			_private->_layer_stack.pop_back();
		}
		return !_private->_layer_stack.empty();
	}

	bool Gui::process_key_event(const KeyEvent& event)
	{
		return !_private->_layer_stack.empty() && _private->_layer_stack.back()->process_key(event);
	}

	bool Gui::push_layer(const StaticString& name)
	{
		const auto i = _private->_layers.find(String(name, ByReference()));
		if (i == _private->_layers.end())
			return false;
		_private->_layer_stack.push_back(i->second.get());
		i->second->run_action(GuiLayer::Action::Push, _private->_script_context);
		return true;
	}

	void Gui::render(Renderer& renderer, const PointF& cursor) const
	{
		if (_private->_layer_stack.empty())
			return;
		const auto top_layer = std::prev(_private->_layer_stack.end());
		auto layer = top_layer;
		while (layer != _private->_layer_stack.begin() && (*layer)->is_transparent())
			--layer;
		while (layer != top_layer)
			(*layer++)->render(renderer, nullptr);
		(*layer)->render(renderer, &cursor);
	}

	void Gui::set_canvas_handler(const StaticString& name, const std::function<void(Renderer&, const RectF&)>& handler)
	{
		_private->_canvas_handlers[String(name, &_private->_allocator)] = handler;
	}

	Gui::~Gui() = default;
}
