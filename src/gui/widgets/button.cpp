#include "button.h"

#include <yttrium/audio/sound.h>
#include <yttrium/exceptions.h>
#include <yttrium/renderer/renderer.h>
#include <yttrium/static_string.h>
#include "../property_loader.h"

namespace Yttrium
{
	ButtonWidget::ButtonWidget(GuiPrivate& gui, std::string_view name, GuiPropertyLoader& loader)
		: Widget{gui, name}
	{
		if (!loader.load_rect("position"_s, _rect)
			|| !_foreground.load(loader))
			throw GuiDataError{"Bad 'button'"_s};

		loader.load_translatable("text"_s, _text);

		if (_styles[0].background.load(loader))
		{
			for (WidgetStateType i = 1; i < WidgetStateCount; ++i)
				_styles[i].background = _styles[0].background;
		}

		if (loader.load_color("text_color"_s, &_styles[0].text_color))
		{
			for (WidgetStateType i = 1; i < WidgetStateCount; ++i)
				_styles[i].text_color = _styles[0].text_color;
		}

		loader.load_state("state"_s, &_state);

		_sound = loader.load_sound("sound"_s);

		Style* style = &_styles[WidgetStateType(WidgetState::Active)];

		loader.bind("active"_s);
		loader.load_color("text_color"_s, &style->text_color);
		style->background.update(loader);

		style = &_styles[WidgetStateType(WidgetState::Pressed)];

		loader.bind("pressed"_s);
		loader.load_color("text_color"_s, &style->text_color);
		style->background.update(loader);

		style = &_styles[WidgetStateType(WidgetState::Checked)];

		loader.bind("checked"_s);
		loader.load_color("text_color"_s, &style->text_color);
		style->background.update(loader);

		style = &_styles[WidgetStateType(WidgetState::Disabled)];

		loader.bind("disabled"_s);
		loader.load_color("text_color"_s, &style->text_color);
		style->background.update(loader);

		loader.unbind();

		_on_click = loader.load_actions("on_click"_s);
	}

	void ButtonWidget::draw(Renderer& renderer, const RectF& rect, WidgetState state) const
	{
		if (_state != WidgetState::NotSet)
			state = _state;
		_styles[WidgetStateType(state)].background.draw(renderer, rect);
		_foreground.color = _styles[WidgetStateType(state)].text_color;
		_foreground.prepare(_text, rect);
		_foreground.draw(renderer);
	}

	bool ButtonWidget::process_mouse_press(Key key, const Vector2&)
	{
		return _state != WidgetState::Checked && _state != WidgetState::Disabled && key == Key::Mouse1;
	}

	void ButtonWidget::process_mouse_release()
	{
		if (_sound)
			_sound->play();
		_on_click.run(_gui);
	}
}
