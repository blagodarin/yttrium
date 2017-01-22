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
	class AudioPlayer;
	class Gui;
	class GuiLayer;
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

		GuiPrivate(ResourceLoader&, ScriptContext&, AudioPlayer&, Allocator&);
		~GuiPrivate();

		GuiLayer& add_layer(const StaticString& name, bool is_transparent, bool is_root);
		Allocator& allocator() const { return _allocator; }
		GuiCursor default_cursor() const { return _default_cursor; }
		void draw_custom_cursor(Renderer& renderer, const PointF& point) const { if (_custom_cursor_handler) _custom_cursor_handler(renderer, point); }
		const FontDesc* font(const StaticString& name) const;
		bool pop_layer();
		bool pop_layers_until(const StaticString& name);
		bool push_layer(const StaticString& name);
		void quit() const { if (_quit_handler) _quit_handler(); }
		void render_canvas(Renderer&, const StaticString& name, const RectF&) const;
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
		AudioPlayer& _audio_player;
		Allocator& _allocator;
		StdMap<String, FontDesc> _fonts{ _allocator };
		StdMap<StaticString, UniquePtr<GuiLayer>> _layers;
		GuiLayer* _root_layer = nullptr;
		StdVector<GuiLayer*> _layer_stack{ _allocator };
		StdMap<String, std::function<void(Renderer&, const RectF&)>> _canvas_handlers{ _allocator };
		std::function<void()> _quit_handler;
		ResourcePtr<const Translation> _translation;
		std::map<Key, std::pair<GuiActions, GuiActions>> _on_key;
		GuiCursor _default_cursor = GuiCursor::None;
		std::function<void(Renderer&, const PointF&)> _custom_cursor_handler;

		friend Gui;
	};
}

#endif
