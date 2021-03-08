// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "image.h"

namespace Yt
{
	ImageWidget::ImageWidget(IonGuiPrivate& gui, std::unique_ptr<WidgetData>&& data)
		: Widget{ gui, std::move(data) }
	{
	}

	void ImageWidget::draw(RenderPass& pass, const RectF& rect, WidgetData::StyleData& style_data) const
	{
		style_data._background.draw(pass, rect);
	}
}
