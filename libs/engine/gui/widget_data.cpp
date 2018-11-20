#include "widget_data.h"

#include <yttrium/gui/texture_font.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/renderer/pass.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/renderer/textured_rect.h>

#include <cassert>

namespace
{
	using namespace Yttrium;

	SizeF make_text_size(const TextureFont& font, std::string_view text, float max_width, float max_height)
	{
		if (text.empty())
			return { 0, max_height };
		const SizeF unscaled_text_size{ font.text_size(text) };
		const auto font_size = std::min(max_height, unscaled_text_size._height * max_width / unscaled_text_size._width);
		return { unscaled_text_size._width * font_size / unscaled_text_size._height, font_size };
	}

	Vector2 make_top_left(const RectF& rect, const SizeF& size, float margin, unsigned alignment)
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
		case TopLeftAlignment: return { x_left(), y_top() };
		case TopAlignment: return { x_center(), y_top() };
		case TopRightAlignment: return { x_right(), y_top() };
		case LeftAlignment: return { x_left(), y_center() };
		case CenterAlignment: return { x_center(), y_center() };
		case RightAlignment: return { x_right(), y_center() };
		case BottomLeftAlignment: return { x_left(), y_bottom() };
		case BottomAlignment: return { x_center(), y_bottom() };
		case BottomRightAlignment: return { x_right(), y_bottom() };
		}
	}
}

namespace Yttrium
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
		PushTexture push_texture{ pass, font_texture.get(), Texture2D::TrilinearFilter };
		pass.draw_rects(geometry, color);
	}

	void ForegroundProperty::prepare(std::string_view text, const RectF& rect, TextCapture* capture)
	{
		const auto max_text_height = rect.height() * size;
		const auto margins = rect.height() - max_text_height;
		const auto max_text_width = rect.width() - margins;
		if (max_text_height < 1 || max_text_width < 1)
		{
			geometry.clear();
			return;
		}
		const auto& text_size = ::make_text_size(*font, text, max_text_width, max_text_height);
		font->build(geometry, ::make_top_left(rect, text_size, margins, alignment), text_size._height, text, capture);
	}

	void WidgetData::run(GuiPrivate& gui, Action action) const
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
