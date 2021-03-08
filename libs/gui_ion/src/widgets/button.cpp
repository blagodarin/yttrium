// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "button.h"

#include "../gui.h"

namespace Yt
{
	ButtonWidget::ButtonWidget(IonGuiPrivate& gui, std::unique_ptr<WidgetData>&& data)
		: Widget{ gui, std::move(data) }
	{
	}

	void ButtonWidget::draw(RenderPass& pass, const RectF& rect, WidgetData::StyleData& style_data) const
	{
		style_data._background.draw(pass, rect);
		style_data._foreground.prepare(_data->_text, rect);
		style_data._foreground.draw(pass);
	}

	bool ButtonWidget::process_mouse_press(Key key, const Vector2&)
	{
		return !_data->_fixed_style && key == Key::Mouse1;
	}

	void ButtonWidget::process_mouse_release()
	{
		_gui.play_sound(_data->_sound);
		_data->run(_gui, WidgetData::Action::OnClick);
	}
}
