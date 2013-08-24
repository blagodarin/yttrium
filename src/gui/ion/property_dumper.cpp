#include "property_dumper.h"

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

void IonPropertyDumper::dump_margins(const StaticString &, const MarginsI &)
{
}

void IonPropertyDumper::dump_position(const StaticString &, const Vector3f &)
{
}

void IonPropertyDumper::dump_rect(const StaticString &, const RectI &)
{
}

void IonPropertyDumper::dump_scaling(const StaticString &, Scaling)
{
}

void IonPropertyDumper::dump_size(const StaticString &, const Vector2f &)
{
}

void IonPropertyDumper::dump_sound(const StaticString &, const Sound &)
{
}

void IonPropertyDumper::dump_state(const StaticString &, WidgetState)
{
}

void IonPropertyDumper::dump_text(const StaticString &, const StaticString &)
{
}

void IonPropertyDumper::dump_texture(const StaticString &, const Texture2DPtr &)
{
}

} // namespace Gui

} // namespace Yttrium
