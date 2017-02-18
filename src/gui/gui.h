#ifndef _src_gui_gui_h_
#define _src_gui_gui_h_

#include <yttrium/resources/resource_ptr.h>
#include <yttrium/static_string.h>
#include "actions.h"
#include "cursor.h"
#include "key_lookup.h"

#include <functional>
#include <map>
#include <memory>
#include <unordered_map>

namespace Yttrium
{
	class Gui;
	class GuiScreen;
	class Music;
	class PointF;
	class RectF;
	class Renderer;
	class ResourceLoader;
	class ScriptContext;
	class Storage;
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
		};

		GuiPrivate(ResourceLoader&, ScriptContext&, Allocator&);
		~GuiPrivate();

		GuiScreen& add_screen(const std::string& name, bool is_transparent, bool is_root);
		Allocator& allocator() const { return _allocator; }
		void draw_canvas(Renderer& renderer, const std::string& name, const RectF& rect) const { if (_on_canvas) _on_canvas(renderer, name, rect); }
		void draw_custom_cursor(Renderer& renderer, const PointF& point) const { if (_on_custom_cursor) _on_custom_cursor(renderer, point); }
		const FontDesc* font(const std::string& name) const;
		bool pop_screen();
		bool pop_screens_until(const std::string& name);
		bool push_screen(const std::string& name);
		void quit() const { if (_on_quit) _on_quit(); }
		ResourceLoader& resource_loader() const { return _resource_loader; }
		ScriptContext& script_context() const { return _script_context; }
		void set_default_cursor(GuiCursor, const StaticString& texture = {});
		void set_font(const std::string& name, const StaticString& font_source, const StaticString& texture_name);
		void set_on_key(const StaticString& key, GuiActions&& on_press, GuiActions&& on_release) { _on_key[lookup_key(key)] = std::make_pair(std::move(on_press), std::move(on_release)); }
		void set_translation(const StaticString& path);
		std::string translate(const StaticString&) const;

	private:
		void enter_screen(GuiScreen&);
		void leave_screen();

	private:
		ResourceLoader& _resource_loader;
		ScriptContext& _script_context;
		Allocator& _allocator;
		std::unordered_map<std::string, FontDesc> _fonts;
		std::unordered_map<std::string, std::unique_ptr<GuiScreen>> _screens; // TODO: Use std::string_view in C++17.
		GuiScreen* _root_screen = nullptr;
		std::vector<GuiScreen*> _screen_stack;
		ResourcePtr<const Translation> _translation;
		std::map<Key, std::pair<GuiActions, GuiActions>> _on_key;
		GuiCursor _default_cursor = GuiCursor::None;
		ResourcePtr<const Texture2D> _default_cursor_texture;
		std::function<void(Renderer&, const std::string&, const RectF&)> _on_canvas;
		std::function<void(Renderer&, const PointF&)> _on_custom_cursor;
		std::function<void(const ResourcePtr<const Music>&)> _on_music;
		std::function<void()> _on_quit;

		friend Gui;
	};
}

#endif
