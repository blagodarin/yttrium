#ifndef _src_gui_widgets_widget_h_
#define _src_gui_widgets_widget_h_

#include <yttrium/key.h>
#include <yttrium/math/rect.h>
#include <yttrium/string.h>
#include "../types.h"

namespace Yttrium
{
	class GuiImpl;
	class GuiPropertyLoader;
	class Renderer;

	class Widget
	{
	public:

		enum
		{
			CanHaveFocus = 1 << 0,
		};

		Widget(const GuiImpl&, unsigned flags = 0);
		virtual ~Widget() = default;

		unsigned flags() const { return _flags; }
		bool is_enabled() const { return _is_enabled; }
		bool is_focused() const { return _is_focused; }
		StaticString name() const { return _name; }
		RectF rect() const { return _rect; }
		RectF render_rect() const { return _render_rect; }
		void set_focused(bool focused) { _is_focused = focused; }
		void set_name(const StaticString& name) { _name = name; }
		void set_render_rect(const RectF& rect) { _render_rect = rect; }
		void set_text(const StaticString& text) { _text = text; }
		StaticString text() const { return _text; }

		virtual bool load(GuiPropertyLoader& loader) = 0;
		virtual bool process_key(const KeyEvent& event);
		virtual void render(Renderer& renderer, const RectF& rect, const Vector2& scale, WidgetState state) const = 0;

	protected:
		const GuiImpl& _gui;
		RectF          _rect;
		bool           _is_enabled = true;
		String         _text;
		String         _name;
		const unsigned _flags;
		bool           _is_focused = false;
		RectF          _render_rect;
	};
}

#endif
