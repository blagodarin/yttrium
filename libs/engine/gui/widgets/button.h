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

#ifndef _src_gui_widgets_button_h_
#define _src_gui_widgets_button_h_

#include "../widget.h"

namespace Yttrium
{
	class ButtonWidget : public Widget
	{
	public:
		ButtonWidget(GuiPrivate&, std::string_view name, std::unique_ptr<WidgetData>&&);

		void draw(RenderPass&, const RectF&, WidgetData::StyleData&) const override;
		bool process_mouse_press(Key, const Vector2&) override;
		void process_mouse_release() override;
	};
}

#endif
