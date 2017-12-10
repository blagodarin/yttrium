#ifndef _src_gui_gui_h_
#define _src_gui_gui_h_

#include "actions.h"
#include "cursor.h"
#include "key_lookup.h"

#include <functional>
#include <map>
#include <memory>
#include <unordered_map>

// TODO: GUI music cache (can't use ResourceLoader cache because MusicReader isn't reusable).

namespace Yttrium
{
	class Canvas;
	class Gui;
	class GuiScreen;
	class MusicReader;
	class RectF;
	class RenderContext;
	class ResourceLoader;
	class ScriptContext;
	class Storage;
	class Texture2D;
	class TextureFont;
	class Translation;
	class Vector2;

	class GuiPrivate
	{
	public:
		struct FontDesc
		{
			std::shared_ptr<const TextureFont> font;
			std::shared_ptr<const Texture2D> texture;
		};

		GuiPrivate(ResourceLoader&, ScriptContext&);
		~GuiPrivate();

		GuiScreen& add_screen(std::string_view name, bool is_transparent, bool is_root);
		void draw_custom_cursor(RenderContext& context, const Vector2& point) const { if (_on_custom_cursor) _on_custom_cursor(context, point); }
		const FontDesc* font(const std::string& name) const;
		void on_canvas_draw(RenderContext&, const std::string& name, const RectF&) const;
		void on_canvas_mouse_move(const std::string& name, const RectF&, const Vector2&);
		bool on_canvas_mouse_press(const std::string& name, const RectF&, Key, const Vector2&);
		bool pop_screen();
		bool pop_screens_until(const std::string& name);
		bool push_screen(const std::string& name);
		void quit() const { if (_on_quit) _on_quit(); }
		ResourceLoader& resource_loader() const { return _resource_loader; }
		ScriptContext& script_context() const { return _script_context; }
		void set_default_cursor(GuiCursor, std::string_view texture = {});
		void set_font(const std::string& name, std::string_view font_source, std::string_view texture_name);
		void set_on_key(std::string_view key, GuiActions&& on_press, GuiActions&& on_release) { _on_key.insert_or_assign(lookup_key(key), std::make_pair(std::move(on_press), std::move(on_release))); }
		void set_translation(std::string_view);
		std::string translate(std::string_view) const;

	private:
		void enter_screen(GuiScreen&);
		void leave_screen();

	private:
		ResourceLoader& _resource_loader;
		ScriptContext& _script_context;
		std::unordered_map<std::string, FontDesc> _fonts;
		std::unordered_map<std::string_view, std::unique_ptr<GuiScreen>> _screens;
		GuiScreen* _root_screen = nullptr;
		std::vector<GuiScreen*> _screen_stack;
		std::shared_ptr<const Translation> _translation;
		std::map<Key, std::pair<GuiActions, GuiActions>> _on_key;
		GuiCursor _default_cursor = GuiCursor::None;
		std::shared_ptr<const Texture2D> _default_cursor_texture;
		std::unordered_map<std::string, Canvas*> _canvases;
		std::function<void(RenderContext&, const Vector2&)> _on_custom_cursor;
		std::function<void(const std::shared_ptr<MusicReader>&)> _on_music;
		std::function<void()> _on_quit;

		friend Gui;
	};
}

#endif
