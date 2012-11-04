#include "property_dumper.h"

#include <Yttrium/ion/object.h>
#include <Yttrium/ion/value.h>

namespace Yttrium
{

namespace Gui
{

IonPropertyDumper::IonPropertyDumper(Ion::Object *object)
	: _object(object)
{
}

void IonPropertyDumper::dump_alignment(const StaticString &name, Alignment alignment)
{
	// TODO: Implement.
}

void IonPropertyDumper::dump_color(const StaticString &name, const Vector4f &color)
{
	// TODO: Implement.
}

void IonPropertyDumper::dump_position(const StaticString &name, const Vector3f &position)
{
	// TODO: Implement.
}

void IonPropertyDumper::dump_scaling(const StaticString &name, Scaling scaling)
{
	// TODO: Implement.
}

void IonPropertyDumper::dump_size(const StaticString &name, const Vector2f &size)
{
	// TODO: Implement.
}

void IonPropertyDumper::dump_texture(const StaticString &name, const Texture2D &texture)
{
	// TODO: Implement.
}

} // namespace Gui

} // namespace Yttrium
