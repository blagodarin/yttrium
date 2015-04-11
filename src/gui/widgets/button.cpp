#include "button.h"

#include <yttrium/audio/sound.h>
#include <yttrium/log.h>
#include <yttrium/renderer.h>
#include "../property_dumper.h"
#include "../property_loader.h"

#include <utility> // move

namespace Yttrium
{
	Button::Button(Allocator* allocator)
		: Widget(allocator)
		, _state(WidgetState::NotSet)
	{
	}

	void Button::dump(GuiPropertyDumper& dumper) const
	{
		dumper.dump_rect("position", _position);
		dumper.dump_scaling("scale", _scaling);

		// TODO: Dump font.

		dumper.dump_size("text_size", _text_size);
		dumper.dump_text("text", _text);

		if (_state != WidgetState::NotSet)
			dumper.dump_state("state", _state); // NOTE: This would dump not the default state, but the current one. Is it OK?

		// TODO: Dump styles.
	}

	bool Button::load(GuiPropertyLoader& loader)
	{
		if (!(loader.load_rect("position", &_position)
			&& loader.load_font("font", &_font, &_font_texture)
			&& loader.load_size("text_size", &_text_size)))
		{
			Y_LOG("[Gui.Button] Unable to load");
			return false;
		}

		loader.load_scaling("scale", &_scaling);
		loader.load_text("text", &_text);

		if (_styles[0].background.load(loader))
		{
			for (WidgetStateType i = 1; i < WidgetStateCount; ++i)
				_styles[i].background = _styles[0].background;
		}

		if (loader.load_color("text_color", &_styles[0].text_color))
		{
			for (WidgetStateType i = 1; i < WidgetStateCount; ++i)
				_styles[i].text_color = _styles[0].text_color;
		}

		if (loader.load_state("state", &_state)) // Fixed state button.
		{
			_is_enabled = false;
		}

		String action_script(_name.allocator());
		loader.load_text("action", &action_script);
		_action = ScriptCode(std::move(action_script));

		_sound = loader.load_sound("sound");

		Style* style = &_styles[WidgetStateType(WidgetState::Active)];

		loader.bind("active");
		loader.load_color("text_color", &style->text_color);
		style->background.update(loader);

		style = &_styles[WidgetStateType(WidgetState::Pressed)];

		loader.bind("pressed");
		loader.load_color("text_color", &style->text_color);
		style->background.update(loader);

		style = &_styles[WidgetStateType(WidgetState::Checked)];

		loader.bind("checked");
		loader.load_color("text_color", &style->text_color);
		style->background.update(loader);

		style = &_styles[WidgetStateType(WidgetState::Disabled)];

		loader.bind("disabled");
		loader.load_color("text_color", &style->text_color);
		style->background.update(loader);

		_rect = RectF(_position);

		return true;
	}

	bool Button::process_key(const KeyEvent& event)
	{
		if (event.key != Key::Mouse1)
			return false;

		if (event.pressed)
			return true;

		if (_sound)
			_sound->play();
		_action.execute();
		return true;
	}

	void Button::render(Renderer& renderer, const RectF& rect, const Vector2f& scale, WidgetState state) const
	{
		if (_state != WidgetState::NotSet)
			state = _state;

		_styles[WidgetStateType(state)].background.draw(renderer, rect);

		if (_text.is_empty())
			return;

		renderer.set_color(_styles[WidgetStateType(state)].text_color);
		renderer.set_texture(_font_texture);

		if (renderer.set_font(_font))
		{
			renderer.set_font_size(_text_size.x * scale.y, _text_size.y);
			renderer.draw_text(rect.center(), _text, CenterAlignment);
		}
	}
}
