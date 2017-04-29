#ifndef _src_gui_widgets_widget_h_
#define _src_gui_widgets_widget_h_

#include <yttrium/flags.h>
#include <yttrium/key.h>
#include <yttrium/math/rect.h>
#include "../types.h"

namespace Yttrium
{
	class GuiPrivate;
	class GuiPropertyLoader;
	class Renderer;

	class Widget
	{
	public:
		enum class Flag
		{
			CanHaveFocus = 1 << 0,
		};

		virtual ~Widget() = default;

		Flags<Flag> flags() const { return _flags; }
		bool is_focused() const { return _is_focused; }
		RectF rect() const { return _rect; }
		RectF render_rect() const { return _render_rect; }
		void set_focused(bool focused) { _is_focused = focused; }
		void set_render_rect(const RectF& rect) { _render_rect = rect; }

		virtual void draw(Renderer&, const RectF&, WidgetState) const = 0;
		virtual bool process_key(const KeyEvent&) { return false; }
		virtual bool process_mouse_press(Key, const Vector2&) { return false; }
		virtual void process_mouse_release() {}

	protected:
		explicit Widget(GuiPrivate&, Flags<Flag> = {});

	protected:
		GuiPrivate& _gui;
		RectF _rect;

	private:
		const Flags<Flag> _flags;
		bool _is_focused = false;
		RectF _render_rect;
	};
}

#endif
