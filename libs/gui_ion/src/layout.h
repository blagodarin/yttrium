// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/math/size.h>
#include <yttrium/math/vector.h>

#include <memory>
#include <vector>

namespace Yt
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
		void update_cursor(const Vector2& cursor, const Widget* except);

	private:
		const Placement _placement;
		SizeF _size;
		Vector2 _scaling{ 1, 1 };
		std::vector<std::unique_ptr<Widget>> _widgets;
	};
}
