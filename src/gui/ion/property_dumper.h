#ifndef __GUI_ION_PROPERTY_DUMPER_H
#define __GUI_ION_PROPERTY_DUMPER_H

#include <Yttrium/ion/node.h>
#include <Yttrium/renderer/texture_cache.h>

#include "../property_dumper.h"

namespace Yttrium
{

namespace Gui
{

class IonPropertyDumper: public PropertyDumper
{
public:

	IonPropertyDumper(Ion::Object *object);

public: // PropertyDumper

	virtual void dump_alignment(const StaticString &name, Alignment alignment);

	virtual void dump_color(const StaticString &name, const Vector4f &color);

	virtual void dump_position(const StaticString &name, const Vector3f &color);

	virtual void dump_scaling(const StaticString &name, Scaling scaling);

	virtual void dump_size(const StaticString &name, const Vector2f &size);

	virtual void dump_text(const StaticString &name, const StaticString &text);

	virtual void dump_texture(const StaticString &name, const Texture2D &texture);

private:

	Ion::Object *_object;
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_ION_PROPERTY_DUMPER_H
