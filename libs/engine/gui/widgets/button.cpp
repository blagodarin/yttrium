//
// Copyright 2018 Sergei Blagodarin
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

#include "button.h"

#include <yttrium/audio/sound.h>

namespace Yttrium
{
	ButtonWidget::ButtonWidget(GuiPrivate& gui, std::string_view name, std::unique_ptr<WidgetData>&& data)
		: Widget{ gui, name, std::move(data) }
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
		if (_data->_sound)
			_data->_sound->play();
		_data->run(_gui, WidgetData::Action::OnClick);
	}
}
