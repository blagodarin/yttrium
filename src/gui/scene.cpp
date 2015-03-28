#include "scene.h"

#include <yttrium/allocator.h>
#include <yttrium/assert.h>
#include <yttrium/renderer.h>

#include "../memory/allocatable.h"
#include "gui.h"
#include "widgets/button.h"
#include "widgets/canvas.h"
#include "widgets/image.h"
#include "widgets/input.h"
#include "widgets/label.h"

namespace Yttrium
{

GuiScene::GuiScene(GuiImpl& gui, const StaticString& name, Allocator* allocator)
	: _allocator(allocator)
	, _gui(gui)
	, _name(name, allocator)
	, _scaling(Scaling::Stretch)
	, _is_cursor_set(false)
	, _mouse_widget(nullptr)
	, _left_click_widget(nullptr)
	, _focus_widget(nullptr)
	, _is_transparent(false)
	, _bindings(allocator)
{
}

GuiScene::~GuiScene()
{
	for (Widget* widget: _widgets)
		Y_DELETE(_allocator, widget);
}

void GuiScene::load_widget(const StaticString& type, const StaticString& name, GuiPropertyLoader& loader)
{
	Allocatable<Widget> widget(_allocator);

	if (type == "button")
		widget.reset<Button>();
	else if (type == "canvas")
		widget.reset<Canvas>(_gui.callbacks());
	else if (type == "image")
		widget.reset<GuiImage>();
	else if (type == "input")
		widget.reset<GuiInput>();
	else if (type == "label")
		widget.reset<Label>();

	if (!widget)
		return;

	widget->set_scaling(_scaling);

	if (!widget->load(loader))
		return;

	if (!name.is_empty())
	{
		widget->set_name(name);
		_named_widgets[String(name, _allocator)] = widget.get();
	}

	_widgets.push_back(widget.release());
}

bool GuiScene::process_key(const KeyEvent& event)
{
	if (event.pressed && event.key >= Key::Mouse1 && event.key <= Key::Mouse5
		&& _focus_widget && _focus_widget != _mouse_widget)
	{
		_focus_widget->set_focused(false);
		_focus_widget = nullptr;
	}

	bool processed = false;

	if (event.key == Key::Mouse1)
	{
		if (_mouse_widget)
		{
			if (event.pressed)
			{
				processed = _mouse_widget->process_key(event);
				if (processed)
				{
					_left_click_widget = _mouse_widget;
					if (_mouse_widget->flags() & Widget::CanHaveFocus && _mouse_widget != _focus_widget)
					{
						Y_ASSERT(!_focus_widget);
						_focus_widget = _mouse_widget;
						_focus_widget->set_focused(true);
					}
				}
			}
			else if (_mouse_widget == _left_click_widget)
			{
				processed = _mouse_widget->process_key(event);
			}
		}

		if (!event.pressed)
			_left_click_widget = nullptr;
	}
	else if (_focus_widget)
	{
		processed = _focus_widget->process_key(event);
	}

	if (processed)
		return true;

	if (event.pressed && _bindings.call(event.key))
	{
		if (_focus_widget)
		{
			_focus_widget->set_focused(false);
			_focus_widget = nullptr;
		}
		return true;
	}

	return false;
}

void GuiScene::render(Renderer& renderer, const Vector2f& size)
{
	Vector2f scale = size / _size;
	Vector2f shift((size.x - _size.x * scale.y) * .5f, (size.y - _size.y * scale.x) * .5f);

	for (Widget* widget: _widgets)
		widget->update();

	Widget* mouse_widget = nullptr;

	if (_is_cursor_set)
	{
		for (auto i = _widgets.rbegin(); i != _widgets.rend(); ++i)
		{
			if (map((*i)->rect(), shift, scale, (*i)->scaling()).contains(_cursor))
			{
				mouse_widget = *i;
				break;
			}
		}
	}

	_mouse_widget = mouse_widget;

	for (const Widget* widget: _widgets)
	{
		WidgetState state = WidgetState::Normal;
		if (widget == _mouse_widget)
			state = (widget == _left_click_widget) ? WidgetState::Pressed : WidgetState::Active;
		widget->render(renderer, map(widget->rect(), shift, scale, widget->scaling()), scale, state);
	}

	_is_cursor_set = false;
}

RectF GuiScene::map(const RectF& source, const Vector2f& shift, const Vector2f& scale, Scaling scaling) const
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

} // namespace Yttrium
