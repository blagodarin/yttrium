#ifndef _src_gui_layer_h_
#define _src_gui_layer_h_

#include <yttrium/bindings.h>
#include <yttrium/math/rect.h>
#include <yttrium/std/map.h>
#include "layout.h"

namespace Yttrium
{
	class GuiImpl;
	class GuiLayout;
	class Renderer;
	class Widget;

	template <typename> class Pointer;

	class GuiLayer
	{
	public:
		enum class Action
		{
			Push,
			Pop,
		};

		GuiLayer(GuiImpl& gui, const StaticString& name, bool is_transparent);
		~GuiLayer();

		bool is_transparent() const { return _is_transparent; }
		const String& name() const { return _name; }

		void bind(const StaticString& name, const StaticString& action)
		{
			_bindings.bind(name, action);
		}

		GuiLayout& add_layout(GuiLayout::Placement);
		bool process_key(const KeyEvent& event);
		void register_widget(Widget&);
		void render(Renderer& renderer, const PointF* cursor);
		void run_action(Action, ScriptContext&) const;
		void set_action(Action, ScriptCode&&);

	private:
		Widget* widget_at(const PointF&) const;

	private:
		GuiImpl&                   _gui;
		String                     _name;
		StdVector<Pointer<GuiLayout>> _layouts;
		StdMap<StaticString, Widget*> _named_widgets;
		StdVector<Widget*>         _widgets;
		Widget*                    _mouse_widget = nullptr;
		const Widget*              _left_click_widget = nullptr;
		Widget*                    _focus_widget = nullptr;
		const bool                 _is_transparent;
		Bindings                   _bindings;
		ScriptCode                 _on_push;
		ScriptCode                 _on_pop;
	};
}

#endif
