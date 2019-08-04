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

#pragma once

#include "../widget.h"

namespace Yt
{
	class CanvasWidget : public Widget
	{
	public:
		CanvasWidget(GuiPrivate&, std::unique_ptr<WidgetData>&&, std::string_view name);

		void draw(RenderPass&, const RectF&, WidgetData::StyleData&) const override;
		void process_mouse_move(const Vector2&) override;
		bool process_mouse_press(Key, const Vector2&) override;

	private:
		const std::string _name;
	};
}
