#include "properties.h"

#include <yttrium/renderer.h>
#include <yttrium/renderer_modifiers.h>
#include <yttrium/string.h>
#include <yttrium/texture.h>
#include <yttrium/textured_rect.h>
#include "property_loader.h"

#include <cassert>

namespace Yttrium
{
	namespace
	{
		SizeF make_text_size(const TextureFont& font, const StaticString& text, float max_width, float max_height)
		{
			const SizeF unscaled_text_size(font.text_size(text));
			if (text.is_empty())
				return {0, max_height};
			const auto font_size = min(max_height, unscaled_text_size.height() * max_width / unscaled_text_size.width());
			return {unscaled_text_size.width() * font_size / unscaled_text_size.height(), font_size};
		}

		PointF make_top_left(const RectF& rect, const SizeF& size, float margin, unsigned alignment)
		{
			const auto x_left = [&]{ return rect.left() + margin; };
			const auto x_center = [&]{ return (rect.left() + rect.right() - size.width()) / 2; };
			const auto x_right = [&]{ return rect.right() - margin - size.width(); };
			const auto y_top = [&]{ return rect.top() + margin; };
			const auto y_center = [&]{ return (rect.top() + rect.bottom() - size.height()) / 2; };
			const auto y_bottom = [&]{ return rect.bottom() - margin - size.height(); };

			switch (alignment)
			{
			default: assert(false);
			case TopLeftAlignment:     return { x_left(),   y_top()    };
			case TopAlignment:         return { x_center(), y_top()    };
			case TopRightAlignment:    return { x_right(),  y_top()    };
			case LeftAlignment:        return { x_left(),   y_center() };
			case CenterAlignment:      return { x_center(), y_center() };
			case RightAlignment:       return { x_right(),  y_center() };
			case BottomLeftAlignment:  return { x_left(),   y_bottom() };
			case BottomAlignment:      return { x_center(), y_bottom() };
			case BottomRightAlignment: return { x_right(),  y_bottom() };
			}
		}
	}

	void BackgroundProperty::draw(Renderer& renderer, const RectF& rect) const
	{
		renderer.set_color(color);
		PushTexture push_texture(renderer, texture.get());
		renderer.set_texture_rect(texture_rect, borders);
		renderer.draw_rect(rect);
	}

	bool BackgroundProperty::load(const GuiPropertyLoader& loader)
	{
		loader.load_color("color"_s, &color);
		if (loader.load_texture("texture"_s, &texture))
		{
			if (!loader.load_rect("texture_rect"_s, texture_rect))
				texture_rect = RectF(texture->rect());
			loader.load_margins("borders"_s, &borders);
		}
		return true;
	}

	void BackgroundProperty::update(const GuiPropertyLoader& loader)
	{
		loader.load_color("color"_s, &color);
		loader.load_texture("texture"_s, &texture);
		loader.load_rect("texture_rect"_s, texture_rect, true);
		loader.load_margins("borders"_s, &borders);
	}

	ForegroundProperty::ForegroundProperty(Allocator& allocator)
		: geometry(allocator)
	{
	}

	ForegroundProperty::~ForegroundProperty() = default;

	void ForegroundProperty::draw(Renderer& renderer) const
	{
		PushTexture push_texture(renderer, font_texture.get());
		renderer.set_color(color);
		renderer.draw_rects(geometry);
	}

	bool ForegroundProperty::load(const GuiPropertyLoader& loader)
	{
		if (!loader.load_font("font"_s, &font, &font_texture))
			return false;
		loader.load("text_size"_s, size);
		loader.load_color("text_color"_s, &color);
		loader.load_alignment("align"_s, &alignment);
		return true;
	}

	void ForegroundProperty::prepare(const StaticString& text, const RectF& rect, TextCapture* capture)
	{
		const auto max_text_height = rect.height() * size;
		const auto margins = rect.height() - max_text_height;
		const auto max_text_width = rect.width() - margins;
		if (max_text_height < 1 || max_text_width < 1)
			return;
		const auto& text_size = make_text_size(*font, text, max_text_width, max_text_height);
		font->build(geometry, make_top_left(rect, text_size, margins, alignment), text_size.height(), text, SizeF(font_texture->size()), capture);
	}
}
