#ifndef _src_gui_screen_h_
#define _src_gui_screen_h_

#include <yttrium/math/rect.h>
#include <yttrium/static_string.h>
#include "actions.h"
#include "cursor.h"
#include "key_lookup.h"
#include "layout.h"

#include <map>
#include <unordered_map>

namespace Yttrium
{
	class GuiLayout;
	class GuiPrivate;
	class Music;
	class Renderer;
	class Texture2D;
	class Widget;

	class GuiScreen
	{
	public:
		GuiScreen(GuiPrivate&, const std::string& name, bool is_transparent);
		~GuiScreen();

		GuiLayout& add_layout(GuiLayout::Placement);
		void draw(Renderer&, const Vector2* cursor);
		void handle_enter();
		bool handle_event(const std::string&) const;
		bool handle_key(const KeyEvent&);
		void handle_return() const { _on_return.run(_gui); }
		bool is_transparent() const { return _is_transparent; }
		const std::shared_ptr<const Music>& music() const { return _music; }
		const std::string& name() const { return _name; }
		void register_widget(Widget&);
		void set_cursor(GuiCursor, const StaticString& texture = {});
		void set_cursor(GuiCursor cursor, const std::shared_ptr<const Texture2D>& texture) { _cursor = cursor; _cursor_texture = texture; }
		void set_music(const std::shared_ptr<const Music>& music) { _music = music; }
		void set_on_enter(GuiActions&& actions) { _on_enter = std::move(actions); }
		void set_on_event(const std::string& event, GuiActions&& actions) { _on_event[event] = std::move(actions); }
		void set_on_key(const StaticString& key, GuiActions&& on_press, GuiActions&& on_release) { _on_key[lookup_key(key)] = std::make_pair(std::move(on_press), std::move(on_release)); }
		void set_on_return(GuiActions&& actions) { _on_return = std::move(actions); }

	private:
		Widget* widget_at(const Vector2&) const;

	private:
		class Activity;

		GuiPrivate& _gui;
		const std::string _name;
		const bool _is_transparent;
		std::vector<std::unique_ptr<GuiLayout>> _layouts;
		std::vector<Widget*> _widgets;
		const std::unique_ptr<Activity> _activity;
		GuiActions _on_enter;
		std::unordered_map<std::string, GuiActions> _on_event;
		std::map<Key, std::pair<GuiActions, GuiActions>> _on_key;
		GuiActions _on_return;
		GuiCursor _cursor = GuiCursor::None;
		std::shared_ptr<const Texture2D> _cursor_texture;
		std::shared_ptr<const Music> _music;
	};
}

#endif
