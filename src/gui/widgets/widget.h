#ifndef _src_gui_widgets_widget_h_
#define _src_gui_widgets_widget_h_

#include <yttrium/key.h>
#include <yttrium/math/rect.h>
#include <yttrium/string.h>
#include "../types.h"

namespace Yttrium
{
	class GuiPrivate;
	class GuiPropertyLoader;
	class Renderer;

	class Widget
	{
	public:
		enum
		{
			CanHaveFocus = 1 << 0,
		};

		virtual ~Widget() = default;

		unsigned flags() const { return _flags; }
		bool is_focused() const { return _is_focused; }
		RectF rect() const { return _rect; }
		RectF render_rect() const { return _render_rect; }
		void set_focused(bool focused) { _is_focused = focused; }
		void set_render_rect(const RectF& rect) { _render_rect = rect; }

		virtual bool load(GuiPropertyLoader&) = 0;
		virtual bool process_key(const KeyEvent&);
		virtual void render(Renderer&, const RectF&, WidgetState) const = 0;

	protected:
		Widget(GuiPrivate&, unsigned flags = 0);

	protected:
		GuiPrivate& _gui;
		RectF _rect;

	private:
		const unsigned _flags;
		bool _is_focused = false;
		RectF _render_rect;
	};
}

#endif
