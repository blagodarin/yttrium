#ifndef __GUI_PROPERTY_H
#define __GUI_PROPERTY_H

#include <yttrium/margins.h>
#include <yttrium/rect.h>
#include <yttrium/renderer/pointers.h>

namespace Yttrium
{

class GuiPropertyDumper;
class GuiPropertyLoader;

struct TextureProperty
{
	Texture2DPtr texture;
	Rect         rect;
	MarginsI     borders;
	// TODO: Add color property value.

	void dump(GuiPropertyDumper* dumper) const;
	bool load(const GuiPropertyLoader& loader);
	void update(const GuiPropertyLoader& loader);
};

// TODO: Add font property (font, text size, text color).

} // namespace Yttrium

#endif // __GUI_PROPERTY_H
