#include "logging.h"

#include "scene.h"

#include <Yttrium/allocator.h>
#include <Yttrium/assert.h>
#include <Yttrium/renderer.h>
#include <Yttrium/script/manager.h>

#include "manager.h"
#include "widgets/button.h"
#include "widgets/canvas.h"
#include "widgets/image.h"
#include "widgets/label.h"

namespace Yttrium
{

namespace Gui
{

Scene::Scene(ManagerImpl *manager, const StaticString &name, Allocator *allocator)
	: _allocator(allocator)
	, _manager(manager)
	, _name(name, allocator)
	, _scaling(Scaling::Stretch)
	, _is_cursor_set(false)
	, _focused_widget(nullptr)
	, _left_button_widget(nullptr)
	, _is_transparent(false)
	, _bindings(allocator)
{
}

Scene::~Scene()
{
	for (Widgets::iterator i = _widgets.begin(); i != _widgets.end(); ++i)
	{
		Y_DELETE(_allocator, *i);
	}
}

void Scene::load_widget(const StaticString &type, const StaticString &name, PropertyLoader &loader)
{
	Widget *widget = nullptr;

	if (type == "button")
	{
		widget = Y_NEW(_allocator, Button)(_allocator);
	}
	else if (type == "canvas")
	{
		widget = Y_NEW(_allocator, Canvas)(_manager->callbacks(), _allocator);
	}
	else if (type == "image")
	{
		widget = Y_NEW(_allocator, Image)(_allocator);
	}
	else if (type == "label")
	{
		widget = Y_NEW(_allocator, Label)(_allocator);
	}

	if (widget)
	{
		widget->set_scaling(_scaling);

		if (widget->load(loader))
		{
			if (!name.is_empty())
			{
				widget->set_name(name);
				_named_widgets[String(name, _allocator)] = widget;
			}

			_widgets.push_back(widget);
		}
		else
		{
			Y_DELETE(_allocator, widget);
		}
	}
}

bool Scene::process_key(Key key, KeyState state)
{
	bool result = false;

	if (key == Key::Mouse1)
	{
		if (state)
		{
			if (_focused_widget)
			{
				if (_focused_widget->is_enabled())
				{
					_left_button_widget = _focused_widget;
				}
				result = true;
			}
		}
		else
		{
			if (_focused_widget)
			{
				if (_focused_widget == _left_button_widget)
				{
					StaticString action = _focused_widget->action();

					if (!action.is_empty())
					{
						_focused_widget->play();
						ScriptManager::instance()->root_context().execute(action);
					}
				}
				result = true;
			}
			_left_button_widget = nullptr;
		}
	}

	return state ? _bindings.call(key) : result;
}

void Scene::render(Renderer *renderer, const Vector2f &size)
{
	Vector2f scale = size / _size;
	Vector2f shift((size.x - _size.x * scale.y) * .5f, (size.y - _size.y * scale.x) * .5f);

	for (Widget *widget: _widgets)
	{
		widget->update();
	}

	const Widget *focused_widget = nullptr;

	if (_is_cursor_set)
	{
		for (Widgets::reverse_iterator i = _widgets.rbegin(); i != _widgets.rend(); ++i)
		{
			if (map((*i)->area(), shift, scale, (*i)->scaling()).contains(_cursor))
			{
				focused_widget = *i;
				break;
			}
		}
	}

	WidgetState state;

	if (focused_widget != _focused_widget)
	{
		_focused_widget = focused_widget;
		state = WidgetState::Active;
	}
	else
	{
		state = (_left_button_widget == _focused_widget) ? WidgetState::Pressed : WidgetState::Active;
	}

	for (const Widget *widget: _widgets)
	{
		RectF rect = map(widget->area(), shift, scale, widget->scaling());
		widget->render(renderer, rect, scale, (widget == focused_widget) ? state : WidgetState::Normal);
	}

	_is_cursor_set = false;
}

RectF Scene::map(const RectF &source, const Vector2f &shift, const Vector2f &scale, Scaling scaling) const
{
	RectF result;

	switch (scaling)
	{
	case Scaling::Min:

		if (scale.x < scale.y)
		{
			result.set_left(source.left() * scale.x);
			result.set_top(source.top() * scale.x + shift.y);
			result.set_width(source.width() * scale.x);
			result.set_height(source.height() * scale.x);
		}
		else
		{
			result.set_left(source.left() * scale.y + shift.x);
			result.set_top(source.top() * scale.y);
			result.set_width(source.width() * scale.y);
			result.set_height(source.height() * scale.y);
		}
		break;

	case Scaling::Max:

		if (scale.x > scale.y)
		{
			result.set_left(source.left() * scale.x);
			result.set_top(source.top() * scale.x + shift.y);
			result.set_width(source.width() * scale.x);
			result.set_height(source.height() * scale.x);
		}
		else
		{
			result.set_left(source.left() * scale.y + shift.x);
			result.set_top(source.top() * scale.y);
			result.set_width(source.width() * scale.y);
			result.set_height(source.height() * scale.y);
		}
		break;

	case Scaling::Fit:

		result.set_left(source.left() * scale.x);
		result.set_top(source.top() * scale.y);
		if (scale.x < scale.y)
		{
			result.set_width(source.width() * scale.x);
			result.set_height(source.height() * scale.x);
		}
		else
		{
			result.set_width(source.width() * scale.y);
			result.set_height(source.height() * scale.y);
		}
		break;

	default:

		Y_ASSERT(false);
		// Fallthrough.

	case Scaling::Stretch:

		result.set_left(source.left() * scale.x);
		result.set_top(source.top() * scale.y);
		result.set_width(source.width() * scale.x);
		result.set_height(source.height() * scale.y);
		break;
	}

	return result;
}

} // namespace Gui

} // namespace Yttrium
