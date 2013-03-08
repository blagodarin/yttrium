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

	void dump_alignment(const StaticString &name, Alignment alignment) override;

	void dump_color(const StaticString &name, const Vector4f &color) override;

	void dump_position(const StaticString &name, const Vector3f &color) override;

	void dump_scaling(const StaticString &name, Scaling scaling) override;

	void dump_size(const StaticString &name, const Vector2f &size) override;

	void dump_state(const StaticString &name, WidgetState state) override;

	void dump_text(const StaticString &name, const StaticString &text) override;

	void dump_texture(const StaticString &name, const Texture2D &texture) override;

private:

	Ion::Object *_object;
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_ION_PROPERTY_DUMPER_H
