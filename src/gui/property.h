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
	RectI        rect;
	MarginsI     borders;

	void dump(GuiPropertyDumper* dumper) const;
	bool load(const GuiPropertyLoader& loader);
	void update(const GuiPropertyLoader& loader);
};

} // namespace Yttrium

#endif // __GUI_PROPERTY_H
