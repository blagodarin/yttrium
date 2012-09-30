#ifndef __GUI_ION_PROPERTY_LOADER_H
#define __GUI_ION_PROPERTY_LOADER_H

#include "../property_loader.h"

namespace Yttrium
{

namespace Gui
{

class IonPropertyLoader: public PropertyLoader
{
public:

	IonPropertyLoader();

public: // PropertyLoader

	virtual bool load_alignment(const StaticString &name, Alignment *alignment) const;

	virtual bool load_color(const StaticString &name, Vector4f *color) const;

	virtual bool load_position(const StaticString &name, Vector3f *color) const;

	virtual bool load_scaling(const StaticString &name, Scaling *scaling) const;

	virtual bool load_size(const StaticString &name, Vector2f *size) const;

	virtual bool load_texture(const StaticString &name, Texture2D *texture) const;
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_ION_PROPERTY_LOADER_H
