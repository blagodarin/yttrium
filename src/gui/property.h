#ifndef __GUI_PROPERTY_H
#define __GUI_PROPERTY_H

#include <yttrium/margins.h>
#include <yttrium/rect.h>
#include <yttrium/renderer/pointers.h>

namespace Yttrium
{

namespace Gui
{

struct TextureProperty
{
	Texture2DPtr texture;
	Area         rect;
	MarginsI     borders;
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_PROPERTY_H
