#ifndef __GUI_WIDGETS_WIDGET_H
#define __GUI_WIDGETS_WIDGET_H

#include <yttrium/key.h>
#include <yttrium/rect.h>
#include <yttrium/string.h>

#include "../types.h"

namespace Yttrium
{

class GuiPropertyDumper;
class GuiPropertyLoader;
class Renderer;

class Widget
{
public:

	enum
	{
		CanHaveFocus = 1 << 0,
	};

	Widget(Allocator* allocator, unsigned flags = 0)
		: _scaling(Scaling::Stretch)
		, _is_enabled(true)
		, _text(allocator)
		, _name(allocator)
		, _flags(flags)
		, _is_focused(false)
	{
	}

	virtual ~Widget() = default;

public:

	unsigned flags() const { return _flags; }
	bool is_enabled() const { return _is_enabled; }
	bool is_focused() const { return _is_focused; }
	StaticString name() const { return _name; }
	RectF rect() const { return _rect; }
	Scaling scaling() const { return _scaling; }
	void set_focused(bool focused) { _is_focused = focused; }
	void set_name(const StaticString& name) { _name = name; }
	void set_scaling(Scaling scaling) { _scaling = scaling; }
	void set_text(const StaticString& text) { _text = text; } // TODO: Update the widget.
	StaticString text() const { return _text; }

public:

	virtual void dump(GuiPropertyDumper& dumper) const = 0;
	virtual bool load(GuiPropertyLoader& loader) = 0;
	virtual bool process_key(const KeyEvent& event);
	virtual void render(Renderer& renderer, const RectF& rect, const Vector2f& scale, WidgetState state) const = 0;
	virtual void update();

protected:

	RectF          _rect;
	Scaling        _scaling;
	bool           _is_enabled;
	String         _text;
	String         _name;
	const unsigned _flags;
	bool           _is_focused;
};

} // namespace Yttrium

#endif // __GUI_WIDGETS_WIDGET_H
