#include "button.h"

#include <yttrium/audio/sound.h>
#include <yttrium/log.h>
#include <yttrium/renderer.h>
#include <yttrium/script/context.h>
#include <yttrium/texture.h>
#include "../gui.h"
#include "../property_loader.h"

#include <utility>

namespace Yttrium
{
	constexpr auto ButtonTextSize = 0.875f;

	ButtonWidget::ButtonWidget(const GuiPrivate& gui)
		: Widget(gui)
		, _foreground(_gui.allocator())
	{
	}

	bool ButtonWidget::load(GuiPropertyLoader& loader)
	{
		if (!loader.load_rect("position"_s, _rect)
			|| !_foreground.load(loader))
		{
			Log() << "Failed to load 'button'"_s;
			return false;
		}

		loader.load_translatable("text"_s, &_text);

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

		if (loader.load_state("state"_s, &_state)) // Fixed state button.
		{
			_is_enabled = false;
		}

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

		String on_click(&_gui.allocator());
		loader.load_text("on_click"_s, &on_click);
		_on_click = ScriptCode(std::move(on_click), _gui.script_context().allocator());

		return true;
	}

	bool ButtonWidget::process_key(const KeyEvent& event)
	{
		if (_state == WidgetState::Checked || _state == WidgetState::Disabled)
			return false;

		if (event.key != Key::Mouse1)
			return false;

		if (event.pressed)
			return true;

		if (_sound)
			_sound->play();
		_on_click.execute(_gui.script_context());
		return true;
	}

	void ButtonWidget::render(Renderer& renderer, const RectF& rect, const Vector2&, WidgetState state) const
	{
		if (_state != WidgetState::NotSet)
			state = _state;
		_styles[WidgetStateType(state)].background.draw(renderer, rect);
		_foreground.color = _styles[WidgetStateType(state)].text_color;
		_foreground.prepare(_text, rect);
		_foreground.draw(renderer);
	}
}
