#include "property_dumper.h"

// TODO: Implement.

namespace Yttrium
{

GuiIonPropertyDumper::GuiIonPropertyDumper(Ion::Object* object)
	: _object(object)
{
}

void GuiIonPropertyDumper::dump_alignment(const StaticString&, unsigned)
{
}

void GuiIonPropertyDumper::dump_color(const StaticString&, const Vector4f&)
{
}

void GuiIonPropertyDumper::dump_margins(const StaticString&, const MarginsI&)
{
}

void GuiIonPropertyDumper::dump_position(const StaticString&, const Vector2f&)
{
}

void GuiIonPropertyDumper::dump_rect(const StaticString&, const Rect&)
{
}

void GuiIonPropertyDumper::dump_scaling(const StaticString&, Scaling)
{
}

void GuiIonPropertyDumper::dump_size(const StaticString&, const Vector2f&)
{
}

void GuiIonPropertyDumper::dump_sound(const StaticString&, const Sound&)
{
}

void GuiIonPropertyDumper::dump_state(const StaticString&, WidgetState)
{
}

void GuiIonPropertyDumper::dump_text(const StaticString&, const StaticString&)
{
}

void GuiIonPropertyDumper::dump_texture(const StaticString&, const Texture2DPtr&)
{
}

} // namespace Yttrium
