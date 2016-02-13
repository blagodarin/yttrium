#include "properties.h"

#include <yttrium/renderer.h>
#include <yttrium/string.h>
#include <yttrium/texture.h>
#include "property_loader.h"

namespace Yttrium
{
	void BackgroundProperty::draw(Renderer& renderer, const RectF& rect) const
	{
		renderer.set_color(color);
		PushTexture push_texture(renderer, texture.get());
		renderer.set_texture_rectangle(texture_rect);
		renderer.set_texture_borders(borders);
		renderer.draw_rectangle(rect);
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

	void ForegroundProperty::draw(Renderer& renderer, const StaticString& text, const RectF& rect, Renderer::TextCapture* capture) const
	{
		if (text.is_empty())
			return;
		const auto max_text_height = rect.height() * size;
		const auto margins = rect.height() - max_text_height;
		const auto max_text_width = rect.width() - margins;
		if (max_text_height < 1 || max_text_width < 1)
			return;
		PushTexture push_texture(renderer, font_texture.get());
		if (!renderer.set_font(font))
			return;
		const SizeF actual_text_size(font.text_size(text));
		renderer.set_color(color);
		renderer.set_font_size(min(max_text_height, actual_text_size.height() * max_text_width / actual_text_size.width()), 1);
		switch (alignment)
		{
		case TopLeftAlignment:
			renderer.draw_text({rect.left() + margins / 2, rect.top() + margins / 2}, text, BottomRightAlignment, capture);
			break;
		case TopAlignment:
			renderer.draw_text({rect.center().x(), rect.top() + margins / 2}, text, BottomAlignment, capture);
			break;
		case TopRightAlignment:
			renderer.draw_text({rect.right() - margins / 2, rect.top() + margins / 2}, text, BottomLeftAlignment, capture);
			break;
		case LeftAlignment:
			renderer.draw_text({rect.left() + margins / 2, rect.center().y()}, text, RightAlignment, capture);
			break;
		default:
			renderer.draw_text({rect.center().x(), rect.center().y()}, text, CenterAlignment, capture);
			break;
		case RightAlignment:
			renderer.draw_text({rect.right() - margins / 2, rect.center().y()}, text, LeftAlignment, capture);
			break;
		case BottomLeftAlignment:
			renderer.draw_text({rect.left() + margins / 2, rect.bottom() - margins / 2}, text, TopRightAlignment, capture);
			break;
		case BottomAlignment:
			renderer.draw_text({rect.center().x(), rect.bottom() - margins / 2}, text, TopAlignment, capture);
			break;
		case BottomRightAlignment:
			renderer.draw_text({rect.right() - margins / 2, rect.bottom() - margins / 2}, text, TopLeftAlignment, capture);
			break;
		}
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
}
