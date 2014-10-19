#ifndef __GUI_PROPERTY_H
#define __GUI_PROPERTY_H

#include <yttrium/margins.h>
#include <yttrium/rect.h>
#include <yttrium/renderer/pointers.h>
#include <yttrium/texture_font.h>

namespace Yttrium
{

class GuiPropertyDumper;
class GuiPropertyLoader;
class Renderer;

struct BackgroundProperty
{
	Vector4f     color;
	Texture2DPtr texture;
	Rect         texture_rect;
	MarginsI     borders;

	BackgroundProperty(): color(1, 1, 1) {}

	void dump(GuiPropertyDumper& dumper) const;
	bool load(const GuiPropertyLoader& loader);
	void render(Renderer& renderer, const RectF& rect) const;
	void update(const GuiPropertyLoader& loader);
};

struct ForegroundProperty
{
	TextureFont  font;
	Texture2DPtr font_texture;
	Vector2f     size;
	Vector4f     color;

	ForegroundProperty(): color(1, 1, 1) {}

	void dump(GuiPropertyDumper& dumper) const;
	bool load(const GuiPropertyLoader& loader);
	void update(const GuiPropertyLoader& loader);
};

} // namespace Yttrium

#endif // __GUI_PROPERTY_H
