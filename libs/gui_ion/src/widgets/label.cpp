//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include "label.h"

#include <yttrium/script/context.h>
#include "../gui.h"

namespace Yt
{
	LabelWidget::LabelWidget(GuiPrivate& gui, std::unique_ptr<WidgetData>&& data)
		: Widget{ gui, std::move(data) }
	{
	}

	void LabelWidget::draw(RenderPass& pass, const RectF& rect, WidgetData::StyleData& style_data) const
	{
		_gui.script_context().substitute(_final_text, _data->_text);
		style_data._foreground.prepare(_final_text, rect);
		style_data._foreground.draw(pass);
	}
}
