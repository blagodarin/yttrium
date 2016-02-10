#include "gui.h"

#include <yttrium/log.h>
#include <yttrium/script/context.h>
#include <yttrium/texture.h>
#include <yttrium/texture_cache.h>
#include "../renderer/renderer.h"
#include "ion/loader.h"
#include "layer.h"

namespace Yttrium
{
	GuiImpl::GuiImpl(ScriptContext& script_context, RendererImpl& renderer, WindowCallbacks& callbacks, Allocator& allocator)
		: _script_context(script_context)
		, _renderer(renderer)
		, _callbacks(callbacks)
		, _proxy_allocator("gui"_s, allocator)
		, _texture_cache(TextureCache::create(_renderer))
		, _fonts(_proxy_allocator)
		, _layers(_proxy_allocator)
		, _layer_stack(_proxy_allocator)
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
			_layer_stack.back()->do_pop_action(_script_context);
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
		i->second->do_push_action(_script_context);
		return true;
	}

	bool GuiImpl::add_layer(Pointer<GuiLayer>&& layer, bool is_root)
	{
		const String& layer_name = layer->name();

		if (_layers.find(layer_name) != _layers.end())
		{
			Log() << "(gui) Layer \""_s << layer_name << "\" has already been added"_s;
			return false;
		}

		if (is_root)
		{
			if (!_layer_stack.empty())
			{
				Log() << "(gui) Layer \""_s << layer_name << "\" \"root\" option ignored"_s;
				// TODO: Print the existing root layer name.
			}
			else
			{
				_layer_stack.push_back(layer.get());
			}
		}

		_layers.emplace(layer_name, std::move(layer));

		return true;
	}

	Pointer<GuiLayer> GuiImpl::create_layer(const StaticString& name, bool is_transparent)
	{
		return make_pointer<GuiLayer>(_proxy_allocator, *this, name, is_transparent);
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

	void GuiImpl::render(const Point& cursor)
	{
		if (_layer_stack.empty())
			return;
		auto i = _layer_stack.begin() + (_layer_stack.size() - 1);
		(*i)->set_cursor(PointF(cursor));
		while (i != _layer_stack.begin() && (*i)->is_transparent())
			--i;
		do
		{
			(*i)->render(_renderer);
			++i;
		} while (i != _layer_stack.end());
	}

	void GuiImpl::set_font(const StaticString& name, const StaticString& font_source, const StaticString& texture_name)
	{
		const auto& texture = _texture_cache->load_texture_2d(texture_name, true);
		if (!texture)
			return;

		// NOTE: If one of the manager's fonts is set in the renderer, we have no ways of removing it from there
		// when the manager is being cleant up, so we must use the renderer's allocator here.

		TextureFont font(font_source, &_renderer.allocator());

		if (font)
		{
			texture->set_filter(Texture2D::TrilinearFilter | Texture2D::AnisotropicFilter);
			_fonts[String(name, &_proxy_allocator)] = FontDesc(std::move(font), texture);
		}
	}
}
