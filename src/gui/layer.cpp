#include "layer.h"

#include <yttrium/math/margins.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/resources/resource_loader.h>
#include "gui.h"
#include "layout.h"
#include "widgets/widget.h"

#include <cassert>

namespace Yttrium
{
	GuiLayer::GuiLayer(GuiPrivate& gui, const StaticString& name, bool is_transparent)
		: _gui(gui)
		, _name(name, &_gui.allocator())
		, _layouts(_gui.allocator())
		, _widgets(_gui.allocator())
		, _is_transparent(is_transparent)
	{
	}

	GuiLayer::~GuiLayer() = default;

	GuiLayout& GuiLayer::add_layout(GuiLayout::Placement placement)
	{
		_layouts.emplace_back(make_unique<GuiLayout>(_gui.allocator(), _gui, placement));
		return *_layouts.back();
	}

	bool GuiLayer::handle_event(const std::string& event) const
	{
		const auto i = _on_event.find(event);
		if (i == _on_event.end())
			return false;
		i->second.run(_gui);
		return true;
	}

	bool GuiLayer::handle_key(const KeyEvent& event)
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

	void GuiLayer::register_widget(Widget& widget)
	{
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
		if (cursor)
		{
			switch (_cursor)
			{
			case GuiCursor::Custom:
				_gui.draw_custom_cursor(renderer, *cursor);
				break;
			case GuiCursor::Texture:
				{
					PushTexture push_texture(renderer, _cursor_texture.get(), Texture2D::TrilinearFilter);
					renderer.set_texture_rect({ {}, SizeF{ _cursor_texture->size() } }, {});
					renderer.draw_rect({ *cursor, SizeF{ _cursor_texture->size() } });
				}
				break;
			default:
				break;
			}
		}
	}

	void GuiLayer::set_cursor(GuiCursor cursor, const StaticString& texture)
	{
		_cursor = cursor;
		if (_cursor == GuiCursor::Texture)
			_cursor_texture = _gui.resource_loader().load_texture_2d(texture);
	}

	Widget* GuiLayer::widget_at(const PointF& point) const
	{
		for (auto i = _widgets.rbegin(); i != _widgets.rend(); ++i)
			if ((*i)->render_rect().contains(point))
				return *i;
		return nullptr;
	}
}
