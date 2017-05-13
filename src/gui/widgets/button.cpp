#include "button.h"

#include <yttrium/audio/sound.h>

namespace Yttrium
{
	ButtonWidget::ButtonWidget(GuiPrivate& gui, std::string_view name, std::unique_ptr<WidgetData>&& data)
		: Widget{gui, name, std::move(data)}
	{
	}

	void ButtonWidget::draw(Renderer& renderer, const RectF& rect, WidgetData::Style style) const
	{
		if (_data->_fixed_style)
			style = *_data->_fixed_style;
		auto& style_data = _data->style_data(style);
		style_data._background.draw(renderer, rect);
		style_data._foreground.prepare(_data->_text, rect);
		style_data._foreground.draw(renderer);
	}

	bool ButtonWidget::process_mouse_press(Key key, const Vector2&)
	{
		return !_data->_fixed_style && key == Key::Mouse1;
	}

	void ButtonWidget::process_mouse_release()
	{
		if (_data->_sound)
			_data->_sound->play();
		_data->run(_gui, WidgetData::Action::OnClick);
	}
}
