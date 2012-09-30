#ifndef __GUI_PROPERTY_LOADER_H
#define __GUI_PROPERTY_LOADER_H

#include <Yttrium/gui/types.h>
#include <Yttrium/renderer/texture.h>
#include <Yttrium/static_string.h>

namespace Yttrium
{

namespace Gui
{

class PropertyLoader
{
public:

	inline virtual ~PropertyLoader();

public:

	// NOTE: These functions MAY modify the data even if they haven't finished successfully.

	virtual bool load_alignment(const StaticString &name, Alignment *alignment) const = 0;

	virtual bool load_color(const StaticString &name, Vector4f *color) const = 0;

	virtual bool load_position(const StaticString &name, Vector3f *color) const = 0;

	virtual bool load_scaling(const StaticString &name, Scaling *scaling) const = 0;

	virtual bool load_size(const StaticString &name, Vector2f *size) const = 0;

	virtual bool load_texture(const StaticString &name, Texture2D *texture) const = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyLoader::~PropertyLoader()
{
}

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_PROPERTY_LOADER_H
