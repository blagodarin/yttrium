#include "gui.h"

#include <yttrium/log.h>
#include <yttrium/script/context.h>
#include <yttrium/texture.h>
#include <yttrium/texture_cache.h>
#include "../renderer/renderer.h"
#include "exceptions.h"
#include "ion/loader.h"
#include "layer.h"

namespace Yttrium
{
	GuiImpl::GuiImpl(ScriptContext& script_context, RendererImpl& renderer, WindowCallbacks& callbacks, Allocator& allocator)
		: _script_context(script_context)
		, _renderer(renderer)
		, _callbacks(callbacks)
		, _allocator("gui"_s, allocator)
		, _texture_cache(TextureCache::create(_renderer))
		, _fonts(_allocator)
		, _layers(_allocator)
		, _layer_stack(_allocator)
	{
	}

	GuiImpl::~GuiImpl()
	{
		clear();
	}

	void GuiImpl::clear()
	{
		_fonts.clear();
		_layers.clear();
		_layer_stack.clear();
	}

	bool GuiImpl::has_layer(const StaticString& name) const
	{
		return _layers.find(String(name, ByReference())) != _layers.end();
	}

	bool GuiImpl::load(const StaticString& filename)
	{
		clear();

		GuiIonLoader loader(*this);
		if (!loader.load(filename))
			return false;

		if (_layer_stack.empty())
		{
			Log() << "(gui) No root layer has been added"_s;
			clear();
			return false;
		}

		return true;
	}

	bool GuiImpl::pop_layers(size_t count)
	{
		for (auto n = min(count, _layer_stack.size()); n > 0; --n)
		{
			_layer_stack.back()->run_action(GuiLayer::Action::Pop, _script_context);
			_layer_stack.pop_back();
		}
		return !_layer_stack.empty();
	}

	bool GuiImpl::push_layer(const StaticString& name)
	{
		auto i = _layers.find(String(name, ByReference()));
		if (i == _layers.end())
			return false;
		_layer_stack.push_back(i->second.get());
		i->second->run_action(GuiLayer::Action::Push, _script_context);
		return true;
	}

	GuiLayer& GuiImpl::add_layer(const StaticString& name, bool is_transparent, bool is_root)
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

	const GuiImpl::FontDesc* GuiImpl::font(const StaticString& name) const
	{
		const auto i = _fonts.find(String(name, ByReference()));
		return i != _fonts.end() ? &i->second : nullptr;
	}

	bool GuiImpl::process_key_event(const KeyEvent& event)
	{
		return !_layer_stack.empty() && _layer_stack.back()->process_key(event);
	}

	void GuiImpl::render(const PointF& cursor) const
	{
		if (_layer_stack.empty())
			return;
		const auto top = _layer_stack.begin() + (_layer_stack.size() - 1);
		auto i = top;
		while (i != _layer_stack.begin() && (*i)->is_transparent())
			--i;
		while (i != top)
			(*i++)->render(_renderer, nullptr);
		(*i)->render(_renderer, &cursor);
	}

	void GuiImpl::set_font(const StaticString& name, const StaticString& font_source, const StaticString& texture_name)
	{
		auto&& texture = _texture_cache->load_texture_2d(texture_name, true);
		if (!texture)
		{
			Log() << "Can't load \""_s << texture_name << "\""_s;
			return;
		}

		auto&& font = TextureFont::load(font_source, _allocator);
		if (!font)
		{
			Log() << "Can't load \""_s << font_source << "\""_s;
			return;
		}

		if (!Rect(texture->size()).contains(font->rect()))
		{
			Log() << "Can't use \""_s << font_source << "\" with \""_s << texture_name << "\""_s;
			return;
		}

		texture->set_filter(Texture2D::TrilinearFilter | Texture2D::AnisotropicFilter);
		_fonts[String(name, &_allocator)] = FontDesc(std::move(font), std::move(texture));
	}
}
