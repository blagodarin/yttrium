#ifndef __GUI_PROPERTY_DUMPER_H
#define __GUI_PROPERTY_DUMPER_H

#include <Yttrium/renderer/texture.h>
#include <Yttrium/static_string.h>

#include "types.h"

namespace Yttrium
{

namespace Gui
{

class PropertyDumper
{
public:

	inline virtual ~PropertyDumper();

public:

	virtual void dump_alignment(const StaticString &name, Alignment alignment) = 0;

	virtual void dump_color(const StaticString &name, const Vector4f &color) = 0;

	virtual void dump_position(const StaticString &name, const Vector3f &color) = 0;

	virtual void dump_scaling(const StaticString &name, Scaling scaling) = 0;

	virtual void dump_size(const StaticString &name, const Vector2f &size) = 0;

	virtual void dump_text(const StaticString &name, const StaticString &text) = 0;

	virtual void dump_texture(const StaticString &name, const Texture2D &texture) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PropertyDumper::~PropertyDumper()
{
}

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_PROPERTY_DUMPER_H
