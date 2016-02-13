#include "layer.h"

#include <yttrium/renderer.h>
#include "exceptions.h"
#include "gui.h"
#include "layout.h"
#include "widgets/widget.h"

#include <cassert>

namespace Yttrium
{
	GuiLayer::GuiLayer(GuiImpl& gui, const StaticString& name, bool is_transparent)
		: _gui(gui)
		, _name(name, &_gui.allocator())
		, _layouts(_gui.allocator())
		, _named_widgets(_gui.allocator())
		, _widgets(_gui.allocator())
		, _is_transparent(is_transparent)
		, _bindings(_gui.script_context(), &_gui.allocator())
	{
	}

	GuiLayer::~GuiLayer()
	{
	}

	GuiLayout& GuiLayer::add_layout(GuiLayout::Placement placement)
	{
		_layouts.emplace_back(make_pointer<GuiLayout>(_gui.allocator(), _gui, placement));
		return *_layouts.back();
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

	void GuiLayer::register_widget(Widget& widget)
	{
		if (!widget.name().is_empty() && !_named_widgets.emplace(widget.name(), &widget).second)
			throw GuiError(_gui.allocator()) << "Duplicate layer \""_s << _name << "\" widget name \""_s << widget.name() << "\""_s;
		_widgets.emplace_back(&widget);
	}

	void GuiLayer::render(Renderer& renderer, const PointF* cursor)
	{
		const RectF rect({}, SizeF(renderer.window_size()));
		for (const auto& layout : _layouts)
			layout->update(rect);
		_mouse_widget = cursor ? widget_at(*cursor) : nullptr;
		for (const auto& layout : _layouts)
			layout->render(renderer, _mouse_widget, _left_click_widget);
	}

	void GuiLayer::run_action(Action action, ScriptContext& context) const
	{
		switch (action)
		{
		case Action::Push:
			_on_push.execute(context);
			break;
		case Action::Pop:
			_on_pop.execute(context);
			break;
		}
	}

	void GuiLayer::set_action(Action action, ScriptCode&& script)
	{
		switch (action)
		{
		case Action::Push:
			_on_push = std::move(script);
			break;
		case Action::Pop:
			_on_pop = std::move(script);
			break;
		}
	}

	Widget* GuiLayer::widget_at(const PointF& point) const
	{
		for (auto i = _widgets.rbegin(); i != _widgets.rend(); ++i)
			if ((*i)->render_rect().contains(point))
				return *i;
		return nullptr;
	}
}
