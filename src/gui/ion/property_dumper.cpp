#include "property_dumper.h"

#include <Yttrium/ion/object.h>
#include <Yttrium/ion/value.h>

// TODO: Implement.

namespace Yttrium
{

namespace Gui
{

IonPropertyDumper::IonPropertyDumper(Ion::Object *object)
	: _object(object)
{
}

void IonPropertyDumper::dump_alignment(const StaticString &, Alignment)
{
}

void IonPropertyDumper::dump_color(const StaticString &, const Vector4f &)
{
}

void IonPropertyDumper::dump_position(const StaticString &, const Vector3f &)
{
}

void IonPropertyDumper::dump_scaling(const StaticString &, Scaling)
{
}

void IonPropertyDumper::dump_size(const StaticString &, const Vector2f &)
{
}

void IonPropertyDumper::dump_state(const StaticString &, WidgetState)
{
}

void IonPropertyDumper::dump_text(const StaticString &, const StaticString &)
{
}

void IonPropertyDumper::dump_texture(const StaticString &, const Texture2D &)
{
}

} // namespace Gui

} // namespace Yttrium
