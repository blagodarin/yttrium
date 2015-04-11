#include "property.h"

#include <yttrium/renderer.h>
#include <yttrium/string.h>
#include <yttrium/texture.h>
#include "property_dumper.h"
#include "property_loader.h"

namespace Yttrium
{
	void BackgroundProperty::draw(Renderer& renderer, const RectF& rect) const
	{
		renderer.set_color(color);
		renderer.set_texture(texture);
		renderer.set_texture_rectangle(RectF(texture_rect));
		renderer.set_texture_borders(borders);
		renderer.draw_rectangle(rect);
	}

	void BackgroundProperty::dump(GuiPropertyDumper& dumper) const
	{
		dumper.dump_color("color", color);
		if (texture)
		{
			dumper.dump_texture("texture", texture);
			dumper.dump_rect("texture_rect", texture_rect);
			dumper.dump_margins("borders", borders);
		}
	}

	bool BackgroundProperty::load(const GuiPropertyLoader& loader)
	{
		loader.load_color("color", &color);
		if (loader.load_texture("texture", &texture))
		{
			if (!loader.load_rect("texture_rect", &texture_rect))
				texture_rect = texture->rect();
			loader.load_margins("borders", &borders);
		}
		return true;
	}

	void BackgroundProperty::update(const GuiPropertyLoader& loader)
	{
		loader.load_texture("texture", &texture);
		loader.load_rect("texture_rect", &texture_rect, true);
		loader.load_margins("borders", &borders);
	}

	void ForegroundProperty::draw(Renderer& renderer, const String& text,
		const Vector2f& origin, unsigned alignment, float scale, Renderer::TextCapture* capture) const
	{
		renderer.set_texture(font_texture);
		if (!renderer.set_font(font))
			return;
		renderer.set_color(color);
		renderer.set_font_size(size.x * scale, size.y);
		renderer.draw_text(origin, text, alignment, capture);
	}

	void ForegroundProperty::dump(GuiPropertyDumper& dumper) const
	{
		if (!font_texture)
			return;
		// TODO: Dump font.
		dumper.dump_size("text_size", size);
		dumper.dump_color("text_color", color);
	}

	bool ForegroundProperty::load(const GuiPropertyLoader& loader)
	{
		if (!loader.load_font("font", &font, &font_texture))
			return false;
		if (!loader.load_size("text_size", &size))
			return false;
		loader.load_color("text_color", &color);
		return true;
	}
}
