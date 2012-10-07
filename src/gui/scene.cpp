#include "logging.h"

#include "scene.h"

#include <Yttrium/allocator.h>
#include <Yttrium/renderer.h>
#include <Yttrium/script/manager.h>

#include "image.h"

namespace Yttrium
{

namespace Gui
{

Scene::Scene(Allocator *allocator)
	: _allocator(allocator)
	, _scaling(Scaling::Stretch)
	, _is_cursor_set(false)
	, _focused_widget(nullptr)
	, _left_button_widget(nullptr)
	, _is_transparent(false)
{
}

Scene::~Scene()
{
	for (Widgets::iterator i = _widgets.begin(); i != _widgets.end(); ++i)
	{
		_allocator->delete_(*i);
	}
}

void Scene::add_widget(const StaticString &type, const StaticString &name, const PropertyLoader &loader)
{
	Widget *widget = nullptr;

	if (type == "image")
	{
		widget = new(_allocator->allocate<Image>())
			Image();
	}

	if (widget)
	{
		if (widget->load(loader))
		{
			widget->set_scaling(_scaling);

			if (!name.is_empty())
			{
				widget->set_name(name);
				_named_widgets[name] = widget;
			}

			_widgets.push_back(widget);
		}
		else
		{
			_allocator->delete_(widget);
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

	for (Widgets::iterator i = _widgets.begin(); i != _widgets.end(); ++i)
	{
		(*i)->update();
	}

	Widget *focused_widget = nullptr;

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

	for (Widgets::iterator i = _widgets.begin(); i != _widgets.end(); ++i)
	{
		RectF rect = map((*i)->area(), shift, scale, (*i)->scaling());
		(*i)->render(renderer, rect, scale, (*i == focused_widget) ? state : WidgetState::Normal);
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

	default: // case Scaling::Stretch:

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
