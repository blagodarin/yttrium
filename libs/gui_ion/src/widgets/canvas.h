// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../widget.h"

namespace Yt
{
	class CanvasWidget : public Widget
	{
	public:
		CanvasWidget(IonGuiPrivate&, std::unique_ptr<WidgetData>&&, std::string_view name);

		void draw(RenderPass&, const RectF&, WidgetData::StyleData&) const override;
		void process_mouse_move(const Vector2&) override;
		bool process_mouse_press(Key, const Vector2&) override;

	private:
		const std::string _name;
	};
}
