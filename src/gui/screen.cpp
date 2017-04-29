#include "screen.h"

#include <yttrium/math/margins.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/resource_loader.h>
#include "gui.h"
#include "layout.h"
#include "widgets/widget.h"

#include <cassert>

namespace Yttrium
{
	GuiScreen::GuiScreen(GuiPrivate& gui, const std::string& name, bool is_transparent)
		: _gui(gui)
		, _name(name)
		, _is_transparent(is_transparent)
	{
	}

	GuiScreen::~GuiScreen() = default;

	GuiLayout& GuiScreen::add_layout(GuiLayout::Placement placement)
	{
		_layouts.emplace_back(std::make_unique<GuiLayout>(_gui, placement));
		return *_layouts.back();
	}

	void GuiScreen::draw(Renderer& renderer, const Vector2* cursor)
	{
		const RectF rect{{}, SizeF{renderer.window_size()}};
		for (const auto& layout : _layouts)
			layout->update(rect);
		_mouse_widget = cursor ? widget_at(*cursor) : nullptr;
		for (const auto& layout : _layouts)
			layout->draw(renderer, _mouse_widget, _left_click_widget);
		if (cursor)
		{
			switch (_cursor)
			{
			case GuiCursor::Custom:
				_gui.draw_custom_cursor(renderer, *cursor);
				break;
			case GuiCursor::Texture:
				{
					PushTexture push_texture{renderer, _cursor_texture.get(), Texture2D::TrilinearFilter};
					renderer.set_texture_rect({{}, SizeF{_cursor_texture->size()}}, {});
					renderer.draw_rect({*cursor, SizeF{_cursor_texture->size()}});
				}
				break;
			default:
				break;
			}
		}
	}

	void GuiScreen::handle_enter()
	{
		_on_enter.run(_gui);

		std::vector<Widget*> focusable_widgets;
		std::copy_if(_widgets.begin(), _widgets.end(), std::back_inserter(focusable_widgets), [](Widget* widget){ return widget->flags() & Widget::Flag::CanHaveFocus; });
		if (!focusable_widgets.empty())
		{
			_focus_widget = focusable_widgets.front();
			_focus_widget->set_focused(true);
		}
	}

	bool GuiScreen::handle_event(const std::string& event) const
	{
		const auto i = _on_event.find(event);
		if (i == _on_event.end())
			return false;
		i->second.run(_gui);
		return true;
	}

	bool GuiScreen::handle_key(const KeyEvent& event)
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
						if (_mouse_widget->flags() & Widget::Flag::CanHaveFocus && _mouse_widget != _focus_widget)
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

		if (!event.autorepeat)
		{
			const auto i = _on_key.find(event.key);
			if (i != _on_key.end())
			{
				(event.pressed ? i->second.first : i->second.second).run(_gui);
				if (_focus_widget)
				{
					_focus_widget->set_focused(false);
					_focus_widget = nullptr;
				}
				return true;
			}
		}

		return false;
	}

	void GuiScreen::register_widget(Widget& widget)
	{
		_widgets.emplace_back(&widget);
	}

	void GuiScreen::set_cursor(GuiCursor cursor, const StaticString& texture)
	{
		_cursor = cursor;
		if (_cursor == GuiCursor::Texture)
			_cursor_texture = _gui.resource_loader().load_texture_2d(texture);
	}

	Widget* GuiScreen::widget_at(const Vector2& point) const
	{
		for (auto i = _widgets.rbegin(); i != _widgets.rend(); ++i)
			if ((*i)->render_rect().contains(point))
				return *i;
		return nullptr;
	}
}
