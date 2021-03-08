// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../widget.h"

namespace Yt
{
	class ButtonWidget : public Widget
	{
	public:
		ButtonWidget(IonGuiPrivate&, std::unique_ptr<WidgetData>&&);

		void draw(RenderPass&, const RectF&, WidgetData::StyleData&) const override;
		bool process_mouse_press(Key, const Vector2&) override;
		void process_mouse_release() override;
	};
}
