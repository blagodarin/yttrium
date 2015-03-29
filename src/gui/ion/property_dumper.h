#ifndef __GUI_ION_PROPERTY_DUMPER_H
#define __GUI_ION_PROPERTY_DUMPER_H

#include "../property_dumper.h"

namespace Yttrium
{
	class IonObject;

	class GuiIonPropertyDumper: public GuiPropertyDumper
	{
	public:

		GuiIonPropertyDumper(IonObject* object);

		void dump_alignment(const StaticString& name, unsigned alignment) override;
		void dump_color(const StaticString& name, const Vector4f& color) override;
		void dump_margins(const StaticString& name, const MarginsI& margins) override;
		void dump_position(const StaticString& name, const Vector2f& position) override;
		void dump_rect(const StaticString& name, const Rect& rect) override;
		void dump_scaling(const StaticString& name, Scaling scaling) override;
		void dump_size(const StaticString& name, const Vector2f& size) override;
		void dump_sound(const StaticString& name, const Sound& sound) override;
		void dump_state(const StaticString& name, WidgetState state) override;
		void dump_text(const StaticString& name, const StaticString& text) override;
		void dump_texture(const StaticString& name, const Pointer<Texture2D>& texture) override;

	private:

		IonObject* _object;
	};
}

#endif // __GUI_ION_PROPERTY_DUMPER_H
