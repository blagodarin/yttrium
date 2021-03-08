// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "widget_data.h"

#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/pass.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/renderer/textured_rect.h>

#include <cassert>

namespace
{
	Yt::SizeF make_text_size(const Yt::Font& font, std::string_view text, float max_width, float max_height)
	{
		if (text.empty())
			return { 0, max_height };
		const Yt::SizeF unscaled_text_size{ font.text_size(text) };
		const auto font_size = std::min(max_height, unscaled_text_size._height * max_width / unscaled_text_size._width);
		return { unscaled_text_size._width * font_size / unscaled_text_size._height, font_size };
	}

	Yt::Vector2 make_top_left(const Yt::RectF& rect, const Yt::SizeF& size, float margin, unsigned alignment)
	{
		const auto x_left = [&] { return rect.left() + margin; };
		const auto x_center = [&] { return (rect.left() + rect.right() - size._width) / 2; };
		const auto x_right = [&] { return rect.right() - margin - size._width; };
		const auto y_top = [&] { return rect.top() + margin; };
		const auto y_center = [&] { return (rect.top() + rect.bottom() - size._height) / 2; };
		const auto y_bottom = [&] { return rect.bottom() - margin - size._height; };

		switch (alignment)
		{
		default:
#ifndef NDEBUG
			assert(false);
#else
			[[fallthrough]];
#endif
		case Yt::TopLeftAlignment: return { x_left(), y_top() };
		case Yt::TopAlignment: return { x_center(), y_top() };
		case Yt::TopRightAlignment: return { x_right(), y_top() };
		case Yt::LeftAlignment: return { x_left(), y_center() };
		case Yt::CenterAlignment: return { x_center(), y_center() };
		case Yt::RightAlignment: return { x_right(), y_center() };
		case Yt::BottomLeftAlignment: return { x_left(), y_bottom() };
		case Yt::BottomAlignment: return { x_center(), y_bottom() };
		case Yt::BottomRightAlignment: return { x_right(), y_bottom() };
		}
	}
}

namespace Yt
{
	void BackgroundProperty::draw(RenderPass& pass, const RectF& rect) const
	{
		PushTexture push_texture{ pass, texture.get(), texture_filter };
		pass.set_texture_rect(texture_rect, borders);
		pass.draw_rect(rect, color);
	}

	ForegroundProperty::ForegroundProperty() = default;
	ForegroundProperty::~ForegroundProperty() = default;

	void ForegroundProperty::draw(RenderPass& pass) const
	{
		PushTexture push_texture{ pass, _font_graphics._texture.get(), Texture2D::TrilinearFilter };
		pass.draw_rects(_font_graphics._glyphs, _color);
	}

	void ForegroundProperty::prepare(std::string_view text, const RectF& rect, TextCapture* capture)
	{
		const auto max_text_height = rect.height() * _size;
		const auto margins = rect.height() - max_text_height;
		const auto max_text_width = rect.width() - margins;
		if (max_text_height < 1 || max_text_width < 1)
		{
			_font_graphics._glyphs.clear();
			return;
		}
		const auto& text_size = ::make_text_size(*_font, text, max_text_width, max_text_height);
		_font->build(_font_graphics, ::make_top_left(rect, text_size, margins, _alignment), text_size._height, text, capture);
	}

	void WidgetData::run(IonGuiPrivate& gui, Action action) const
	{
		const auto i = _actions.find(action);
		if (i != _actions.end())
			i->second.run(gui);
	}

	WidgetData::StyleData& WidgetData::style_data(Style style)
	{
		const auto i = _styles.find(style);
		return i != _styles.end() ? i->second : _styles[Style::Normal];
	}
}
