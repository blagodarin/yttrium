#ifndef _src_gui_layer_h_
#define _src_gui_layer_h_

#include <yttrium/math/rect.h>
#include <yttrium/resources/resource_ptr.h>
#include "actions.h"
#include "cursor.h"
#include "key_lookup.h"
#include "layout.h"

#include <map>
#include <unordered_map>

#include <boost/optional/optional.hpp>

namespace Yttrium
{
	class GuiLayout;
	class GuiPrivate;
	class Music;
	class Renderer;
	class Texture2D;
	class Widget;

	template <typename> class UniquePtr;

	class GuiLayer
	{
	public:
		GuiLayer(GuiPrivate&, const StaticString& name, bool is_transparent);
		~GuiLayer();

		GuiLayout& add_layout(GuiLayout::Placement);
		void handle_enter();
		bool handle_event(const std::string&) const;
		bool handle_key(const KeyEvent&);
		void handle_return() const { _on_return.run(_gui); }
		bool has_music() const noexcept { return static_cast<bool>(_music); }
		bool is_transparent() const { return _is_transparent; }
		ResourcePtr<const Music> music() const { return *_music; }
		const String& name() const { return _name; }
		void register_widget(Widget&);
		void render(Renderer&, const PointF* cursor);
		void set_cursor(GuiCursor, const StaticString& texture = {});
		void set_cursor(GuiCursor cursor, const ResourcePtr<const Texture2D>& texture) { _cursor = cursor; _cursor_texture = texture; }
		void set_music(const ResourcePtr<const Music>& music) { _music = music; }
		void set_on_enter(GuiActions&& actions) { _on_enter = std::move(actions); }
		void set_on_event(const std::string& event, GuiActions&& actions) { _on_event[event] = std::move(actions); }
		void set_on_key(const StaticString& key, GuiActions&& on_press, GuiActions&& on_release) { _on_key[lookup_key(key)] = std::make_pair(std::move(on_press), std::move(on_release)); }
		void set_on_return(GuiActions&& actions) { _on_return = std::move(actions); }

	private:
		Widget* widget_at(const PointF&) const;

	private:
		GuiPrivate& _gui;
		const String _name;
		const bool _is_transparent;
		std::vector<UniquePtr<GuiLayout>> _layouts;
		std::vector<Widget*> _widgets;
		Widget* _mouse_widget = nullptr;
		const Widget* _left_click_widget = nullptr;
		Widget* _focus_widget = nullptr;
		GuiActions _on_enter;
		std::unordered_map<std::string, GuiActions> _on_event;
		std::map<Key, std::pair<GuiActions, GuiActions>> _on_key;
		GuiActions _on_return;
		GuiCursor _cursor = GuiCursor::None;
		ResourcePtr<const Texture2D> _cursor_texture;
		boost::optional<ResourcePtr<const Music>> _music;
	};
}

#endif
