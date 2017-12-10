#include "screen.h"

#include <yttrium/math/margins.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/resource_loader.h>
#include "gui.h"
#include "layout.h"
#include "widget.h"

#include <cassert>

namespace Yttrium
{
	class GuiScreen::Activity
	{
	public:
		Widget* click_widget() const { return _click_widget; }
		Widget* hover_widget() const { return _hover_widget; }

		bool process(const KeyEvent& event)
		{
			return event.key >= Key::Mouse1 && event.key <= Key::Mouse5
				? (event.pressed ? on_mouse_press(event.key) : on_mouse_release(event.key))
				: on_key_event(event);
		}

		void reset()
		{
			*this = {};
		}

		void set_focus(Widget* widget)
		{
			if (_focus_widget != widget)
			{
				if (_focus_widget)
				{
					_focus_widget->set_focused(false);
					_focus_widget = nullptr;
				}
				if (widget)
				{
					_focus_widget = widget;
					_focus_widget->set_focused(true);
				}
			}
		}

		void set_hover(const Vector2& cursor, Widget* widget)
		{
			_hover_cursor = cursor;
			_hover_widget = widget;
			if (_click_widget && cursor != _click_cursor)
			{
				_click_cursor.x = std::clamp(cursor.x, _click_widget->render_rect().left(), _click_widget->render_rect().right() - 1);
				_click_cursor.y = std::clamp(cursor.y, _click_widget->render_rect().top(), _click_widget->render_rect().bottom() - 1);
				_click_widget->process_mouse_move(_click_cursor);
			}
		}

	private:
		bool on_key_event(const KeyEvent& event) const
		{
			return _focus_widget && _focus_widget->process_key(event);
		}

		bool on_mouse_press(Key button)
		{
			if (_focus_widget && _focus_widget != _hover_widget)
			{
				_focus_widget->set_focused(false);
				_focus_widget = nullptr;
			}
			if (_click_widget)
			{
				_click_widget->process_mouse_release();
				_click_widget = nullptr;
				_click_button = Key::Null;
				_click_cursor = {0, 0};
			}
			if (!_hover_widget || !_hover_widget->process_mouse_press(button, _hover_cursor))
				return false;
			_click_widget = _hover_widget;
			_click_button = button;
			_click_cursor = _hover_cursor;
			if (_click_widget->flags() & Widget::Flag::CanHaveFocus && _click_widget != _focus_widget)
			{
				assert(!_focus_widget);
				set_focus(_click_widget);
			}
			return true;
		}

		bool on_mouse_release(Key button)
		{
			if (!_click_widget)
				return false;
			if (button == _click_button)
			{
				if (_hover_widget == _click_widget)
					_click_widget->process_mouse_release();
				_click_widget = nullptr;
				_click_button = Key::Null;
				_click_cursor = {0, 0};
			}
			return true;
		}

	private:
		Widget* _hover_widget = nullptr;
		Vector2 _hover_cursor{0, 0};
		Widget* _click_widget = nullptr;
		Key _click_button = Key::Null;
		Vector2 _click_cursor{0, 0};
		Widget* _focus_widget = nullptr;
	};

	GuiScreen::GuiScreen(GuiPrivate& gui, std::string_view name, bool is_transparent)
		: _gui{gui}
		, _name{name}
		, _is_transparent{is_transparent}
		, _activity{std::make_unique<Activity>()}
	{
	}

	GuiScreen::~GuiScreen() = default;

	GuiLayout& GuiScreen::add_layout(GuiLayout::Placement placement)
	{
		return *_layouts.emplace_back(std::make_unique<GuiLayout>(_gui, placement));
	}

	void GuiScreen::draw(RenderContext& context, const Vector2* cursor)
	{
		const RectF rect{{}, context.window_size()};
		for (const auto& layout : _layouts)
			layout->update(rect);
		if (cursor)
			_activity->set_hover(*cursor, widget_at(*cursor));
		else
			_activity->reset();
		for (const auto& layout : _layouts)
			layout->draw(context, _activity->hover_widget(), _activity->click_widget());
		if (cursor)
		{
			switch (_cursor)
			{
			case GuiCursor::Custom:
				_gui.draw_custom_cursor(context, *cursor);
				break;
			case GuiCursor::Texture:
				{
					PushTexture push_texture{context, _cursor_texture.get(), Texture2D::TrilinearFilter};
					context.set_texture_rect({{}, SizeF{_cursor_texture->size()}}, {});
					context.draw_rect({*cursor, SizeF{_cursor_texture->size()}});
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
			_activity->set_focus(focusable_widgets.front());
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
		if (_activity->process(event))
			return true;

		if (!event.autorepeat)
		{
			const auto i = _on_key.find(event.key);
			if (i != _on_key.end())
			{
				(event.pressed ? i->second.first : i->second.second).run(_gui);
				_activity->set_focus(nullptr);
				return true;
			}
		}

		return false;
	}

	void GuiScreen::register_widget(Widget& widget)
	{
		_widgets.emplace_back(&widget);
	}

	void GuiScreen::set_cursor(GuiCursor cursor, std::string_view texture)
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
