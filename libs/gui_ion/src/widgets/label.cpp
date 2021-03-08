// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "label.h"

#include <yttrium/script/context.h>
#include "../gui.h"

namespace Yt
{
	LabelWidget::LabelWidget(IonGuiPrivate& gui, std::unique_ptr<WidgetData>&& data)
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
