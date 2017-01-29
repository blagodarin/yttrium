#ifndef _src_gui_gui_h_
#define _src_gui_gui_h_

#include <yttrium/memory/unique_ptr.h>
#include <yttrium/resources/resource_ptr.h>
#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>
#include "actions.h"
#include "cursor.h"
#include "key_lookup.h"

#include <functional>
#include <memory>

namespace Yttrium
{
	class Gui;
	class GuiLayer;
	class Music;
	class PointF;
	class RectF;
	class Renderer;
	class ResourceLoader;
	class ScriptContext;
	class Storage;
	class String;
	class Texture2D;
	class TextureFont;
	class Translation;

	class GuiPrivate
	{
	public:
		struct FontDesc
		{
			ResourcePtr<const TextureFont> font;
			ResourcePtr<const Texture2D> texture;

			FontDesc() = default;

			FontDesc(ResourcePtr<const TextureFont>&& font, ResourcePtr<const Texture2D>&& texture)
				: font(std::move(font))
				, texture(std::move(texture))
			{
			}
		};

		GuiPrivate(ResourceLoader&, ScriptContext&, Allocator&);
		~GuiPrivate();

		GuiLayer& add_layer(const StaticString& name, bool is_transparent, bool is_root);
		Allocator& allocator() const { return _allocator; }
		GuiCursor default_cursor() const { return _default_cursor; }
		void draw_canvas(Renderer& renderer, const StaticString& name, const RectF& rect) const { if (_on_canvas) _on_canvas(renderer, name, rect); }
		void draw_custom_cursor(Renderer& renderer, const PointF& point) const { if (_on_custom_cursor) _on_custom_cursor(renderer, point); }
		const FontDesc* font(const StaticString& name) const;
		bool pop_layer();
		bool pop_layers_until(const StaticString& name);
		bool push_layer(const StaticString& name);
		void quit() const { if (_on_quit) _on_quit(); }
		ResourceLoader& resource_loader() const { return _resource_loader; }
		ScriptContext& script_context() const { return _script_context; }
		void set_default_cursor(GuiCursor cursor) { _default_cursor = cursor; }
		void set_font(const StaticString& name, const StaticString& font_source, const StaticString& texture_name);
		void set_on_key(const StaticString& key, GuiActions&& on_press, GuiActions&& on_release) { _on_key[lookup_key(key)] = std::make_pair(std::move(on_press), std::move(on_release)); }
		void set_translation(const StaticString& path);
		String translate(const StaticString&) const;

	private:
		void enter_layer(GuiLayer*);
		void leave_layer();

	private:
		ResourceLoader& _resource_loader;
		ScriptContext& _script_context;
		Allocator& _allocator;
		StdMap<String, FontDesc> _fonts{ _allocator };
		StdMap<StaticString, UniquePtr<GuiLayer>> _layers;
		GuiLayer* _root_layer = nullptr;
		StdVector<GuiLayer*> _layer_stack{ _allocator };
		ResourcePtr<const Translation> _translation;
		std::map<Key, std::pair<GuiActions, GuiActions>> _on_key;
		GuiCursor _default_cursor = GuiCursor::None;
		std::function<void(Renderer&, const StaticString&, const RectF&)> _on_canvas;
		std::function<void(Renderer&, const PointF&)> _on_custom_cursor;
		std::function<void(const ResourcePtr<const Music>&)> _on_music;
		std::function<void()> _on_quit;

		friend Gui;
	};
}

#endif
