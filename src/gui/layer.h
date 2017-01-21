#ifndef _src_gui_layer_h_
#define _src_gui_layer_h_

#include <yttrium/math/rect.h>
#include <yttrium/std/map.h>
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
	class Renderer;
	class Widget;

	template <typename> class UniquePtr;

	class GuiLayer
	{
	public:
		GuiLayer(GuiPrivate&, const StaticString& name, bool is_transparent);
		~GuiLayer();

		bool is_transparent() const { return _is_transparent; }
		const String& name() const { return _name; }

		GuiLayout& add_layout(GuiLayout::Placement);
		void handle_enter() const { _on_enter.run(_gui); }
		bool handle_event(const std::string&) const;
		bool handle_key(const KeyEvent&);
		void handle_return() const { _on_return.run(_gui); }
		void register_widget(Widget&);
		void render(Renderer& renderer, const PointF* cursor);
		void set_cursor(GuiCursor cursor) { _cursor = cursor; }
		void set_on_enter(GuiActions&& actions) { _on_enter = std::move(actions); }
		void set_on_event(const std::string& event, GuiActions&& actions) { _on_event[event] = std::move(actions); }
		void set_on_key(const StaticString& key, GuiActions&& on_press, GuiActions&& on_release) { _on_key[lookup_key(key)] = std::make_pair(std::move(on_press), std::move(on_release)); }
		void set_on_return(GuiActions&& actions) { _on_return = std::move(actions); }

	private:
		Widget* widget_at(const PointF&) const;

	private:
		GuiPrivate& _gui;
		String _name;
		StdVector<UniquePtr<GuiLayout>> _layouts;
		StdVector<Widget*> _widgets;
		Widget* _mouse_widget = nullptr;
		const Widget* _left_click_widget = nullptr;
		Widget* _focus_widget = nullptr;
		const bool _is_transparent;
		GuiActions _on_enter;
		std::unordered_map<std::string, GuiActions> _on_event;
		std::map<Key, std::pair<GuiActions, GuiActions>> _on_key;
		GuiActions _on_return;
		GuiCursor _cursor = GuiCursor::None;
	};
}

#endif
