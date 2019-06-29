//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "layout.h"

#include "widget.h"

namespace Yttrium
{
	GuiLayout::GuiLayout(Placement placement)
		: _placement{ placement }
	{
	}

	Widget& GuiLayout::add_widget(std::unique_ptr<Widget>&& widget)
	{
		return *_widgets.emplace_back(std::move(widget));
	}

	void GuiLayout::draw(RenderPass& pass, const Widget* hover_widget, const Widget* click_widget) const
	{
		for (const auto& widget : _widgets)
		{
			auto style = WidgetData::Style::Normal;
			if (widget.get() == hover_widget)
				style = (widget.get() == click_widget) ? WidgetData::Style::Pressed : WidgetData::Style::Hovered;
			widget->draw(pass, widget->render_rect(), style);
		}
	}

	void GuiLayout::update(const RectF& rect)
	{
		auto offset = rect.top_left();
		Vector2 scaling{ 1, 1 };
		switch (_placement)
		{
		case Placement::Left:
			if (_size._height > 0)
			{
				const auto s = rect.height() / _size._height;
				scaling = { s, s };
			}
			break;

		case Placement::Right:
			if (_size._height > 0)
			{
				offset = rect.top_right();
				const auto s = rect.height() / _size._height;
				scaling = { -s, s };
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
					offset = { (rect.width() - width) / 2, 0 };
					scaling = { width / _size._width, rect.height() / _size._height };
				}
				else
				{
					const auto height = rect.width() / layout_aspect;
					offset = { 0, (rect.height() - height) / 2 };
					scaling = { rect.width() / _size._width, height / _size._height };
				}
			}
			break;

		default:
			if (_size._width > 0 && _size._height > 0)
				scaling = { rect.width() / _size._width, rect.height() / _size._height };
		}

		for (const auto& widget : _widgets)
		{
			const auto layout_rect = widget->rect();
			auto screen_rect = layout_rect == RectF{} ? rect : RectF{ offset + layout_rect.top_left() * scaling, layout_rect.size() * scaling };
			if (screen_rect.width() < 0)
				screen_rect = { screen_rect.top_right(), SizeF{ -screen_rect.width(), screen_rect.height() } };
			if (screen_rect.height() < 0)
				screen_rect = { screen_rect.bottom_left(), SizeF{ screen_rect.width(), -screen_rect.height() } };
			widget->set_render_rect(screen_rect);
		}
	}
}
