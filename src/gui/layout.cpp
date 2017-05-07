#include "layout.h"

#include <yttrium/exceptions.h>
#include "gui.h"
#include "widgets/button.h"
#include "widgets/canvas.h"
#include "widgets/image.h"
#include "widgets/input.h"
#include "widgets/label.h"

namespace Yttrium
{
	GuiLayout::GuiLayout(GuiPrivate& gui, Placement placement)
		: _gui{gui}
		, _placement{placement}
	{
	}

	Widget& GuiLayout::add_widget(std::string_view type, std::string_view name, GuiPropertyLoader& loader)
	{
		std::unique_ptr<Widget> widget;
		if (type == "button")
			widget = std::make_unique<ButtonWidget>(_gui, name, loader);
		else if (type == "canvas")
			widget = std::make_unique<CanvasWidget>(_gui, name, loader);
		else if (type == "image")
			widget = std::make_unique<ImageWidget>(_gui, name, loader);
		else if (type == "input")
			widget = std::make_unique<InputWidget>(_gui, name, loader);
		else if (type == "label")
			widget = std::make_unique<LabelWidget>(_gui, name, loader);
		else
			throw GuiDataError{"Unknown widget type '", type, "'"};
		_widgets.emplace_back(std::move(widget));
		return *_widgets.back();
	}

	void GuiLayout::draw(Renderer& renderer, const Widget* hover_widget, const Widget* click_widget) const
	{
		for (const auto& widget : _widgets)
		{
			WidgetState state = WidgetState::Normal;
			if (widget.get() == hover_widget)
				state = (widget.get() == click_widget) ? WidgetState::Pressed : WidgetState::Active;
			widget->draw(renderer, widget->render_rect(), state);
		}
	}

	void GuiLayout::update(const RectF& rect)
	{
		auto offset = rect.top_left();
		Vector2 scaling{1, 1};
		switch (_placement)
		{
		case Placement::Left:
			if (_size._height > 0)
			{
				const auto s = rect.height() / _size._height;
				scaling = {s, s};
			}
			break;

		case Placement::Right:
			if (_size._height > 0)
			{
				offset = rect.top_right();
				const auto s = rect.height() / _size._height;
				scaling = {-s, s};
			}
			break;

		case Placement::Center:
			if (_size._width > 0 && _size._height > 0)
			{
				const auto window_aspect = rect.width() / rect.height();
				const auto layout_aspect = _size._width / _size._height;
				if (window_aspect > layout_aspect)
				{
					const auto width = rect.height() * layout_aspect;
					offset = {(rect.width() - width) / 2, 0};
					scaling = {width / _size._width, rect.height() / _size._height};
				}
				else
				{
					const auto height = rect.width() / layout_aspect;
					offset = {0, (rect.height() - height) / 2};
					scaling = {rect.width() / _size._width, height / _size._height};
				}
			}
			break;

		default:
			if (_size._width > 0 && _size._height > 0)
				scaling = {rect.width() / _size._width, rect.height() / _size._height};
		}

		for (const auto& widget : _widgets)
		{
			const auto layout_rect = widget->rect();
			auto screen_rect = layout_rect == RectF{} ? rect : RectF{offset + layout_rect.top_left() * scaling, layout_rect.size() * scaling};
			if (screen_rect.width() < 0)
				screen_rect = {screen_rect.top_right(), SizeF{-screen_rect.width(), screen_rect.height()}};
			if (screen_rect.height() < 0)
				screen_rect = {screen_rect.bottom_left(), SizeF{screen_rect.width(), -screen_rect.height()}};
			widget->set_render_rect(screen_rect);
		}
	}
}
