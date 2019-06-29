//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include "widget.h"

namespace Yttrium
{
	void Widget::draw(RenderPass& pass, const RectF& rect, WidgetData::Style style) const
	{
		if (_data->_fixed_style)
			style = *_data->_fixed_style;
		return draw(pass, rect, _data->style_data(style));
	}

	Widget::Widget(GuiPrivate& gui, std::unique_ptr<WidgetData>&& data, Flags<Flag> flags)
		: _gui{ gui }
		, _data{ std::move(data) }
		, _flags{ flags }
	{
	}

	Widget::~Widget() noexcept = default;
}
