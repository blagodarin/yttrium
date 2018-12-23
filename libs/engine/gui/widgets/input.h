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

#ifndef _src_gui_widgets_input_h_
#define _src_gui_widgets_input_h_

#include "../logic/line_editor.h"
#include "../widget.h"

#include <chrono>

namespace Yttrium
{
	class InputWidget : public Widget
	{
	public:
		InputWidget(GuiPrivate&, std::string_view name, std::unique_ptr<WidgetData>&&);

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

#endif
