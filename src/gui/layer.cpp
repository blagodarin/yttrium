#include "layer.h"

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
	GuiLayer::GuiLayer(GuiImpl& gui, const StaticString& name, bool is_transparent)
		: _gui(gui)
		, _name(name, &_gui.allocator())
		, _widgets(_gui.allocator())
		, _named_widgets(_gui.allocator())
		, _is_transparent(is_transparent)
		, _bindings(_gui.script_context(), &_gui.allocator())
	{
	}

	GuiLayer::~GuiLayer()
	{
	}

	void GuiLayer::load_widget(const StaticString& type, const StaticString& name, GuiPropertyLoader& loader)
	{
		Pointer<Widget> widget;
		if (type == "button"_s)
			widget = make_pointer<ButtonWidget>(_gui.allocator(), _gui);
		else if (type == "canvas"_s)
			widget = make_pointer<CanvasWidget>(_gui.allocator(), _gui);
		else if (type == "image"_s)
			widget = make_pointer<ImageWidget>(_gui.allocator(), _gui);
		else if (type == "input"_s)
			widget = make_pointer<InputWidget>(_gui.allocator(), _gui);
		else if (type == "label"_s)
			widget = make_pointer<LabelWidget>(_gui.allocator(), _gui);

		if (!widget)
			return;

		widget->set_scaling(_scaling);

		if (!widget->load(loader))
			return;

		if (!name.is_empty())
		{
			widget->set_name(name);
			_named_widgets[String(name, &_gui.allocator())] = widget.get();
		}

		_widgets.emplace_back(std::move(widget));
	}

	bool GuiLayer::process_key(const KeyEvent& event)
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

	void GuiLayer::render(Renderer& renderer)
	{
		const auto& window_size = renderer.window_size();
		const auto& layer_size = _has_size ? _size : SizeF(window_size);

		const Vector2 scale(
			window_size.width() / layer_size.width(),
			window_size.height() / layer_size.height());
		const Vector2 shift(
			(window_size.width() - layer_size.width() * scale.y) * .5f,
			(window_size.height() - layer_size.height() * scale.x) * .5f);

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
			const auto& widget_rect = widget->rect();
			const auto& mapped_rect = widget_rect == RectF()
				? map(RectF({}, layer_size), shift, scale, Scaling::Stretch)
				: map(widget_rect, shift, scale, widget->scaling());
			widget->render(renderer, mapped_rect, scale, state);
		}

		_is_cursor_set = false;
	}

	void GuiLayer::reserve(size_t capacity)
	{
		_widgets.reserve(capacity);
	}

	RectF GuiLayer::map(const RectF& source, const Vector2& shift, const Vector2& scale, Scaling scaling) const
	{
		switch (scaling)
		{
		case Scaling::Min:
			if (scale.x < scale.y)
				return
				{
					{
						source.left() * scale.x,
						source.top() * scale.x + shift.y
					},
					source.size() * std::make_pair(scale.x, scale.x)
				};
			else
				return
				{
					{
						source.left() * scale.y + shift.x,
						source.top() * scale.y
					},
					source.size() * std::make_pair(scale.y, scale.y)
				};

		case Scaling::Max:
			if (scale.x > scale.y)
				return
				{
					{
						source.left() * scale.x,
						source.top() * scale.x + shift.y
					},
					source.size() * std::make_pair(scale.x, scale.x)
				};
			else
				return
				{
					{
						source.left() * scale.y + shift.x,
						source.top() * scale.y
					},
					source.size() * std::make_pair(scale.y, scale.y)
				};

		case Scaling::Fit:
			return
			{
				{
					source.left() * scale.x,
					source.top() * scale.y
				},
				source.size() * (scale.x < scale.y ? std::make_pair(scale.x, scale.x) : std::make_pair(scale.y, scale.y))
			};

		default:
			assert(false);
		case Scaling::Stretch:
			return
			{
				{
					source.left() * scale.x,
					source.top() * scale.y
				},
				source.size() * std::make_pair(scale.x, scale.y)
			};
		}
	}
}
