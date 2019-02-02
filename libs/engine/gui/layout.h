//
// Copyright 2019 Sergei Blagodarin
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

#include <yttrium/math/size.h>
#include <yttrium/math/vector.h>

#include <memory>
#include <vector>

namespace Yttrium
{
	class GuiPropertyLoader;
	class RectF;
	class RenderPass;
	class Widget;

	class GuiLayout
	{
	public:
		enum class Placement
		{
			Stretch,
			Center,
			Left,
			Right,
		};

		explicit GuiLayout(Placement);

		void set_size(const SizeF& size) { _size = size; }

		Widget& add_widget(std::unique_ptr<Widget>&&);

		void draw(RenderPass&, const Widget* hover_widget, const Widget* click_widget) const;
		void update(const RectF&);

	private:
		const Placement _placement;
		SizeF _size;
		Vector2 _scaling{ 1, 1 };
		std::vector<std::unique_ptr<Widget>> _widgets;
	};
}
