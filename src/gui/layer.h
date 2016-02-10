#ifndef _src_gui_layer_h_
#define _src_gui_layer_h_

#include <yttrium/bindings.h>
#include <yttrium/math/rect.h>
#include <yttrium/std/map.h>
#include "layout.h"
#include "types.h"

namespace Yttrium
{
	class GuiImpl;
	class GuiLayout;
	class GuiPropertyLoader;
	class Renderer;
	class Widget;

	template <typename> class Pointer;

	class GuiLayer
	{
	public:

		GuiLayer(GuiImpl& gui, const StaticString& name, bool is_transparent);
		~GuiLayer();

	public:

		void bind(const StaticString& name, const StaticString& action)
		{
			_bindings.bind(name, action);
		}

		void do_pop_action(ScriptContext& context) const
		{
			_on_pop.execute(context);
		}

		void do_push_action(ScriptContext& context) const
		{
			_on_push.execute(context);
		}

		bool is_transparent() const
		{
			return _is_transparent;
		}

		GuiLayout& add_layout(GuiLayout::Placement);

		bool add_widget(Widget*);

		const String& name() const
		{
			return _name;
		}

		bool process_key(const KeyEvent& event);

		void render(Renderer& renderer); // TODO: Make const.

		void set_cursor(const PointF& cursor)
		{
			_is_cursor_set = true;
			_cursor = cursor;
		}

		void set_pop_action(ScriptCode&& action)
		{
			_on_pop = std::move(action);
		}

		void set_push_action(ScriptCode&& action)
		{
			_on_push = std::move(action);
		}

		void set_scaling(Scaling scaling)
		{
			_scaling = scaling;
		}

		void set_size(const SizeF& size)
		{
			_size = size;
			_has_size = true;
		}

	private:
		GuiImpl&                   _gui;
		String                     _name;
		SizeF                      _size;
		bool                       _has_size = false;
		Scaling                    _scaling = Scaling::Stretch;
		StdVector<Pointer<GuiLayout>> _layouts;
		StdMap<StaticString, Widget*> _named_widgets;
		StdVector<Widget*>         _widgets;
		bool                       _is_cursor_set = false;
		PointF                     _cursor;
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
