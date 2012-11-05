#ifndef __GUI_WIDGETS_WIDGET_H
#define __GUI_WIDGETS_WIDGET_H

#include <Yttrium/rect.h>
#include <Yttrium/string.h>

#include "../types.h"

namespace Yttrium
{

class Renderer;

namespace Gui
{

class PropertyDumper;
class PropertyLoader;

class Widget
{
public:

	inline Widget();

	inline virtual ~Widget();

public:

	inline StaticString action() const;

	inline RectF area() const;

	inline bool is_enabled() const;

	inline StaticString name() const;

	inline Scaling scaling() const;

	inline void set_name(const StaticString &name);

	inline void set_scaling(Scaling scaling);

	inline void set_text(const StaticString &name);

	inline StaticString text() const;

public:

	virtual void dump(PropertyDumper *dumper) const = 0;

	virtual bool load(PropertyLoader &loader) = 0;

	virtual void render(Renderer *renderer, const RectF &area, const Vector2f &scale, WidgetState state) const = 0;

	virtual void update();

protected:

	RectF   _area;
	Scaling _scaling;
	bool    _is_enabled;
	String  _action;
	String  _text;
	String  _name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Widget::Widget()
	: _scaling(Scaling::Stretch)
	, _is_enabled(true)
{
}

Widget::~Widget()
{
}

StaticString Widget::action() const
{
	return _action;
}

RectF Widget::area() const
{
	return _area;
}

bool Widget::is_enabled() const
{
	return _is_enabled;
}

StaticString Widget::name() const
{
	return _name;
}

Scaling Widget::scaling() const
{
	return _scaling;
}

void Widget::set_name(const StaticString &name)
{
	_name = name;
}

void Widget::set_scaling(Scaling scaling)
{
	_scaling = scaling;
}

void Widget::set_text(const StaticString &text)
{
	_text = text;
	// TODO: Update.
}

StaticString Widget::text() const
{
	return _text;
}

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_WIDGETS_WIDGET_H
