#include "scene.h"

#include <yttrium/allocator.h>
#include <yttrium/renderer.h>
#include "gui.h"
#include "widgets/button.h"
#include "widgets/canvas.h"
#include "widgets/image.h"
#include "widgets/input.h"
#include "widgets/label.h"

#include <cassert>

namespace Yttrium
{
	GuiScene::GuiScene(GuiImpl& gui, const StaticString& name, bool is_transparent, Allocator* allocator)
		: _allocator(allocator)
		, _gui(gui)
		, _name(name, allocator)
		, _scaling(Scaling::Stretch)
		, _is_cursor_set(false)
		, _mouse_widget(nullptr)
		, _left_click_widget(nullptr)
		, _focus_widget(nullptr)
		, _is_transparent(is_transparent)
		, _bindings(allocator)
	{
	}

	GuiScene::~GuiScene()
	{
	}

	void GuiScene::load_widget(const StaticString& type, const StaticString& name, GuiPropertyLoader& loader)
	{
		Pointer<Widget> widget;
		if (type == "button"_s)
			widget = make_pointer<Button>(*_allocator, _allocator);
		else if (type == "canvas"_s)
			widget = make_pointer<Canvas>(*_allocator, _gui.callbacks(), _allocator);
		else if (type == "image"_s)
			widget = make_pointer<GuiImage>(*_allocator, _allocator);
		else if (type == "input"_s)
			widget = make_pointer<GuiInput>(*_allocator, _allocator);
		else if (type == "label"_s)
			widget = make_pointer<Label>(*_allocator, _allocator);

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

		_widgets.emplace_back(std::move(widget));
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
							assert(!_focus_widget);
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

	void GuiScene::render(Renderer& renderer)
	{
		const auto& window_size = renderer.window_size();
		const Vector2& scene_size = _has_size ? _size : Vector2(window_size);

		const Vector2 scale(
			window_size.width / scene_size.x,
			window_size.height / scene_size.y);
		const Vector2 shift(
			(window_size.width - scene_size.x * scale.y) * .5f,
			(window_size.height - scene_size.y * scale.x) * .5f);

		Widget* mouse_widget = nullptr;

		if (_is_cursor_set)
		{
			for (auto i = _widgets.rbegin(); i != _widgets.rend(); ++i)
			{
				if (map((*i)->rect(), shift, scale, (*i)->scaling()).contains(_cursor))
				{
					mouse_widget = i->get();
					break;
				}
			}
		}

		_mouse_widget = mouse_widget;

		for (const auto& widget : _widgets)
		{
			WidgetState state = WidgetState::Normal;
			if (widget.get() == _mouse_widget)
				state = (widget.get() == _left_click_widget) ? WidgetState::Pressed : WidgetState::Active;
			widget->render(renderer, map(widget->rect(), shift, scale, widget->scaling()), scale, state);
		}

		_is_cursor_set = false;
	}

	void GuiScene::reserve(size_t capacity)
	{
		_widgets.reserve(capacity);
	}

	RectF GuiScene::map(const RectF& source, const Vector2& shift, const Vector2& scale, Scaling scaling) const
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

			assert(false);
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
}
