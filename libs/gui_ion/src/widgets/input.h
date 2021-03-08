// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../logic/line_editor.h"
#include "../widget.h"

#include <chrono>

namespace Yt
{
	class InputWidget : public Widget
	{
	public:
		InputWidget(IonGuiPrivate&, std::unique_ptr<WidgetData>&&);

		void draw(RenderPass&, const RectF&, WidgetData::StyleData&) const override;
		bool process_key(const KeyEvent&) override;
		bool process_text(std::string_view) override;
		bool process_mouse_press(Key, const Vector2&) override;

	private:
		void update_input();

	private:
		LineEditor _logic;
		std::chrono::steady_clock::time_point _cursor_mark;
	};
}
