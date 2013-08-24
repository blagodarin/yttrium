#ifndef __GUI_PROPERTY_H
#define __GUI_PROPERTY_H

#include <yttrium/margins.h>
#include <yttrium/rect.h>
#include <yttrium/renderer/pointers.h>

namespace Yttrium
{

namespace Gui
{

class PropertyDumper;
class PropertyLoader;

struct TextureProperty
{
	Texture2DPtr texture;
	RectI        rect;
	MarginsI     borders;

	void dump(PropertyDumper *dumper) const;
	bool load(PropertyLoader &loader);
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_PROPERTY_H
