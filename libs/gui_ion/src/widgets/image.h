// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../widget.h"

namespace Yt
{
	class ImageWidget : public Widget
	{
	public:
		ImageWidget(IonGuiPrivate&, std::unique_ptr<WidgetData>&&);

		void draw(RenderPass&, const RectF&, WidgetData::StyleData&) const override;
	};
}
