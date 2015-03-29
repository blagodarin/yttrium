#ifndef __GUI_SCENE_H
#define __GUI_SCENE_H

#include <yttrium/bindings.h>
#include <yttrium/rect.h>

#include "types.h"

#include <map>
#include <vector>

namespace Yttrium
{

class GuiImpl;
class GuiIonDumper;
class GuiPropertyLoader;
class Renderer;
class Widget;

class GuiScene
{
	friend GuiIonDumper;

public:

	GuiScene(GuiImpl& gui, const StaticString& name, Allocator* allocator);
	~GuiScene();

public:

	void bind(const StaticString& name, const StaticString& action)
	{
		_bindings.bind(name, action);
	}

	bool is_transparent() const
	{
		return _is_transparent;
	}

	void load_widget(const StaticString& type, const StaticString& name, GuiPropertyLoader& loader);

	const String& name() const
	{
		return _name;
	}

	bool process_key(const KeyEvent& event);

	void render(Renderer& renderer); // TODO: Make const.

	void reserve(size_t capacity)
	{
		_widgets.reserve(capacity);
	}

	void set_cursor(const Vector2f& cursor)
	{
		_is_cursor_set = true;
		_cursor = cursor;
	}

	void set_scaling(Scaling scaling)
	{
		_scaling = scaling;
	}

	void set_size(const Vector2f& size)
	{
		_size = size;
	}

	void set_transparent(bool transparent)
	{
		_is_transparent = transparent;
	}

private:

	RectF map(const RectF& source, const Vector2f& shift, const Vector2f& scale, Scaling scaling) const;

private:

	Allocator*                _allocator;
	GuiImpl&                  _gui;
	String                    _name;
	Vector2f                  _size;
	Scaling                   _scaling;
	std::vector<Widget*>      _widgets;
	std::map<String, Widget*> _named_widgets;
	bool                      _is_cursor_set;
	Vector2f                  _cursor;
	Widget*                   _mouse_widget;
	const Widget*             _left_click_widget;
	Widget*                   _focus_widget;
	bool                      _is_transparent;
	Bindings                  _bindings;
};

} // namespace Yttrium

#endif // __GUI_SCENE_H
