#include "property.h"

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
		renderer.set_texture_rectangle(RectF(texture_rect));
		renderer.set_texture_borders(borders);
		renderer.draw_rectangle(rect);
	}

	bool BackgroundProperty::load(const GuiPropertyLoader& loader)
	{
		loader.load_color("color"_s, &color);
		if (loader.load_texture("texture"_s, &texture))
		{
			if (!loader.load_rect("texture_rect"_s, &texture_rect))
				texture_rect = texture->rect();
			loader.load_margins("borders"_s, &borders);
		}
		return true;
	}

	void BackgroundProperty::update(const GuiPropertyLoader& loader)
	{
		loader.load_color("color"_s, &color);
		loader.load_texture("texture"_s, &texture);
		loader.load_rect("texture_rect"_s, &texture_rect, true);
		loader.load_margins("borders"_s, &borders);
	}

	void ForegroundProperty::draw(Renderer& renderer, const String& text,
		const PointF& origin, unsigned alignment, float scale, Renderer::TextCapture* capture) const
	{
		PushTexture push_texture(renderer, font_texture.get());
		if (!renderer.set_font(font))
			return;
		renderer.set_color(color);
		renderer.set_font_size(size.width() * scale, size.height());
		renderer.draw_text(origin, text, alignment, capture);
	}

	bool ForegroundProperty::load(const GuiPropertyLoader& loader)
	{
		if (!loader.load_font("font"_s, &font, &font_texture))
			return false;
		if (!loader.load_size("text_size"_s, size))
			return false;
		loader.load_color("text_color"_s, &color);
		return true;
	}
}
