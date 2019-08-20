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

#include <yttrium/gui/font.h>
#include <yttrium/math/color.h>
#include <yttrium/math/margins.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/texture.h>
#include "actions.h"

#include <map>
#include <optional>

namespace Yt
{
	class Font;
	class RenderPass;
	class Sound;
	class TextCapture;
	class TexturedRect;

	enum
	{
		CenterAlignment = 0,

		LeftAlignment = 1 << 0,
		RightAlignment = 1 << 1,
		TopAlignment = 1 << 2,
		BottomAlignment = 1 << 3,

		TopLeftAlignment = TopAlignment | LeftAlignment,
		TopRightAlignment = TopAlignment | RightAlignment,
		BottomLeftAlignment = BottomAlignment | LeftAlignment,
		BottomRightAlignment = BottomAlignment | RightAlignment,

		HorizontalAlignmentMask = LeftAlignment | RightAlignment,
		VerticalAlignmentMask = TopAlignment | BottomAlignment,
	};

	struct BackgroundProperty
	{
		Color4f color{ 1, 1, 1 };
		std::shared_ptr<const Texture2D> texture;
		Texture2D::Filter texture_filter = Texture2D::TrilinearFilter;
		RectF texture_rect;
		MarginsF borders;

		BackgroundProperty() = default;

		void draw(RenderPass&, const RectF&) const;
	};

	class ForegroundProperty
	{
	public:
		std::shared_ptr<const Font> _font;
		float _size = .75f;
		Color4f _color{ 1, 1, 1 };
		unsigned _alignment = 0;

		ForegroundProperty();
		ForegroundProperty(const ForegroundProperty&) = default;
		~ForegroundProperty();

		void draw(RenderPass&) const;
		void prepare(std::string_view, const RectF&, TextCapture* = nullptr);

	private:
		Font::Graphics _font_graphics; // TODO: Remove from this class.
	};

	struct WidgetData
	{
		enum class Style
		{
			Normal,
			Hovered,
			Pressed,
			Checked,
			Disabled,
		};

		struct StyleData
		{
			ForegroundProperty _foreground;
			BackgroundProperty _background;
		};

		enum class Action
		{
			OnClick,
			OnUpdate,
			OnEnter,
		};

		RectF _rect;
		std::shared_ptr<Sound> _sound;
		std::map<Style, StyleData> _styles;
		std::map<Action, GuiActions> _actions;
		std::optional<Style> _fixed_style;
		std::string _text;
		bool _track_hover = false;

		void run(GuiPrivate&, Action) const;
		StyleData& style_data(Style);
	};
}
