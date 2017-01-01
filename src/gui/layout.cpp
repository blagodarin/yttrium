#include "layout.h"

#include <yttrium/exceptions.h>
#include <yttrium/static_string.h>
#include "gui.h"
#include "widgets/button.h"
#include "widgets/canvas.h"
#include "widgets/image.h"
#include "widgets/input.h"
#include "widgets/label.h"

namespace Yttrium
{
	GuiLayout::GuiLayout(GuiPrivate& gui, Placement placement)
		: _gui(gui)
		, _placement(placement)
		, _widgets(_gui.allocator())
	{
	}

	Widget& GuiLayout::add_widget(const StaticString& type, GuiPropertyLoader& loader)
	{
		UniquePtr<Widget> widget;
		if (type == "button"_s)
			widget = make_unique<ButtonWidget>(_gui.allocator(), _gui);
		else if (type == "canvas"_s)
			widget = make_unique<CanvasWidget>(_gui.allocator(), _gui);
		else if (type == "image"_s)
			widget = make_unique<ImageWidget>(_gui.allocator(), _gui);
		else if (type == "input"_s)
			widget = make_unique<InputWidget>(_gui.allocator(), _gui);
		else if (type == "label"_s)
			widget = make_unique<LabelWidget>(_gui.allocator(), _gui);
		else
			throw GuiDataError("Unknown widget type '"_s, type, "'"_s);

		if (!widget->load(loader))
			throw GuiDataError("Can't load '"_s, type, "'"_s);

		_widgets.emplace_back(std::move(widget));
		return *_widgets.back();
	}

	void GuiLayout::update(const RectF& rect)
	{
		RectF layout_rect = rect;

		Vector2 scaling(1, 1);
		if (!_size.is_empty())
		{
			if (_placement == Placement::Center)
			{
				const auto window_aspect = rect.width() / rect.height();
				const auto layout_aspect = _size.width() / _size.height();
				if (window_aspect > layout_aspect)
				{
					const auto width = rect.height() * layout_aspect;
					layout_rect = RectF({(rect.width() - width) / 2, 0}, SizeF(width, rect.height()));
				}
				else
				{
					const auto height = rect.width() / layout_aspect;
					layout_rect = RectF({0, (rect.height() - height) / 2}, SizeF(rect.width(), height));
				}
			}
			scaling = Vector2(layout_rect.width() / _size.width(), layout_rect.height() / _size.height());
		}

		for (const auto& widget : _widgets)
		{
			const auto& widget_rect = widget->rect();
			widget->set_render_rect(widget_rect == RectF()
				? layout_rect
				: RectF(
					{
						layout_rect.left() + widget_rect.left() * scaling.x,
						layout_rect.top() + widget_rect.top() * scaling.y
					},
					widget_rect.size() * std::make_pair(scaling.x, scaling.y)));
		}
	}

	void GuiLayout::render(Renderer& renderer, const Widget* hover_widget, const Widget* click_widget) const
	{
		for (const auto& widget : _widgets)
		{
			WidgetState state = WidgetState::Normal;
			if (widget.get() == hover_widget)
				state = (widget.get() == click_widget) ? WidgetState::Pressed : WidgetState::Active;
			widget->render(renderer, widget->render_rect(), state);
		}
	}
}
