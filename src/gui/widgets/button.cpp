#include "../logging.h"

#include "button.h"

#include <yttrium/renderer.h>

#include "../property_dumper.h"
#include "../property_loader.h"

namespace Yttrium
{

namespace Gui
{

Button::Button(Allocator *allocator)
	: Widget(allocator)
	, _state(WidgetState::None)
{
}

void Button::dump(PropertyDumper *dumper) const
{
	dumper->dump_rect("position", _position);
	dumper->dump_scaling("scale", _scaling);

	// TODO: Dump font.

	dumper->dump_size("text_size", _text_size);
	dumper->dump_text("text", _text);

	if (_state != WidgetState::None)
	{
		dumper->dump_state("state", _state); // NOTE: This would dump not the default state, but the current one. Is it OK?
	}

	// TODO: Dump styles.
}

bool Button::load(PropertyLoader &loader)
{
	Y_LOG_TRACE("[Gui.Button] Loading...");

	if (!(loader.load_rect("position", &_position)
		&& loader.load_font("font", &_font, &_font_texture)
		&& loader.load_size("text_size", &_text_size)))
	{
		Y_LOG_DEBUG("[Gui.Button] Unable to load");
		return false;
	}

	loader.load_scaling("scale", &_scaling);
	loader.load_text("text", &_text);

	if (loader.load_color("color", &_styles[0].color))
	{
		for (WidgetStateType i = 1; i < WidgetStateCount; ++i)
		{
			_styles[i].color = _styles[0].color;
		}
	}

	if (_styles[0].texture.load(loader))
	{
		for (WidgetStateType i = 1; i < WidgetStateCount; ++i)
		{
			_styles[i].texture = _styles[0].texture;
		}
	}

	if (loader.load_color("text_color", &_styles[0].text_color))
	{
		for (WidgetStateType i = 1; i < WidgetStateCount; ++i)
		{
			_styles[i].text_color = _styles[0].text_color;
		}
	}

	if (loader.load_state("state", &_state)) // Fixed state button.
	{
		_is_enabled = false;
	}

	loader.load_text("action", &_action);
	_sound = loader.load_sound("sound");

	Style *style = &_styles[WidgetStateType(WidgetState::Active)];

	loader.bind("active");
	loader.load_color("color", &style->color);
	loader.load_color("text_color", &style->text_color);
	style->texture.update(loader);

	style = &_styles[WidgetStateType(WidgetState::Pressed)];

	loader.bind("pressed");
	loader.load_color("color", &style->color);
	loader.load_color("text_color", &style->text_color);
	style->texture.update(loader);

	style = &_styles[WidgetStateType(WidgetState::Checked)];

	loader.bind("checked");
	loader.load_color("color", &style->color);
	loader.load_color("text_color", &style->text_color);
	style->texture.update(loader);

	style = &_styles[WidgetStateType(WidgetState::Disabled)];

	loader.bind("disabled");
	loader.load_color("color", &style->color);
	loader.load_color("text_color", &style->text_color);
	style->texture.update(loader);

	_area = _position;

	return true;
}

void Button::play() const
{
	if (Y_LIKELY(!_sound.is_null()))
	{
		_sound->play();
	}
}

void Button::render(Renderer *renderer, const RectF &area, const Vector2f &scale, WidgetState state) const
{
	if (_state != WidgetState::None)
	{
		state = _state;
	}

	renderer->set_color(_styles[WidgetStateType(state)].color);
	renderer->set_texture(_styles[WidgetStateType(state)].texture.texture);
	renderer->set_texture_rectangle(_styles[WidgetStateType(state)].texture.rect);
	renderer->set_texture_borders(_styles[WidgetStateType(state)].texture.borders);
	renderer->draw_rectangle(area.left(), area.top(), area.width(), area.height());

	if (_text.is_empty())
	{
		return;
	}

	renderer->set_color(_styles[WidgetStateType(state)].text_color);
	renderer->set_texture(_font_texture);

	if (renderer->set_font(_font))
	{
		renderer->set_font_size(_text_size.x * scale.y, _text_size.y);
		renderer->draw_text(area.center(), _text, CenterAlignment);
	}
}

} // namespace Gui

} // namespace Yttrium
