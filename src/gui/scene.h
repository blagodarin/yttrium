#ifndef __GUI_SCENE_H
#define __GUI_SCENE_H

#include <Yttrium/bindings.h>
#include <Yttrium/rect.h>

#include "types.h"

#include <vector>

namespace Yttrium
{

class Renderer;

namespace Gui
{

class IonDumper;
class ManagerImpl;
class PropertyLoader;
class Widget;

///

class Scene
{
	friend class IonDumper;

public:

	Scene(ManagerImpl *manager, const StaticString &name, Allocator *allocator = DefaultAllocator);

	~Scene();

public:

	inline void bind(const StaticString &name, const StaticString &action);

	inline bool is_transparent() const;

	void load_widget(const StaticString &type, const StaticString &name, PropertyLoader &loader);

	inline const String &name() const;

	bool process_key(Key key, KeyState state);

	void render(Renderer *renderer, const Vector2f &scale); // TODO: Make const.

	inline void reserve(size_t capacity);

	inline void set_cursor(const Vector2f &cursor);

	inline void set_scaling(Scaling scaling);

	inline void set_size(const Vector2f &size);

	inline void set_transparent(bool transparent);

private:

	RectF map(const RectF &source, const Vector2f &shift, const Vector2f &scale, Scaling scaling) const;

private:

	typedef std::vector<Widget *>      Widgets;
	typedef std::map<String, Widget *> NamedWidgets;

private:

	Allocator    *_allocator;
	ManagerImpl  *_manager;
	String        _name;
	Vector2f      _size;
	Scaling       _scaling;
	Widgets       _widgets;
	NamedWidgets  _named_widgets;
	bool          _is_cursor_set;
	Vector2f      _cursor;
	Widget       *_focused_widget;
	Widget       *_left_button_widget;
	bool          _is_transparent;
	Bindings      _bindings;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Scene::bind(const StaticString &name, const StaticString &action)
{
	_bindings.bind(name, action);
}

bool Scene::is_transparent() const
{
	return _is_transparent;
}

const String &Scene::name() const
{
	return _name;
}

void Scene::reserve(size_t capacity)
{
	_widgets.reserve(capacity);
}

void Scene::set_cursor(const Vector2f &cursor)
{
	_is_cursor_set = true;
	_cursor = cursor;
}

void Scene::set_scaling(Scaling scaling)
{
	_scaling = scaling;
}

void Scene::set_size(const Vector2f &size)
{
	_size = size;
}

void Scene::set_transparent(bool transparent)
{
	_is_transparent = transparent;
}

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_SCENE_H
