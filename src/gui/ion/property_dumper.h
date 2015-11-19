#ifndef _src_gui_ion_property_dumper_h_
#define _src_gui_ion_property_dumper_h_

#include "../property_dumper.h"

namespace Yttrium
{
	class IonObject;

	class GuiIonPropertyDumper : public GuiPropertyDumper
	{
	public:
		GuiIonPropertyDumper(IonObject& object) : _object(object) {}

		void dump_alignment(const StaticString& name, unsigned) override;
		void dump_color(const StaticString& name, const Vector4&) override;
		void dump_margins(const StaticString& name, const Margins&) override;
		void dump_position(const StaticString& name, const PointF&) override;
		void dump_rect(const StaticString& name, const Rect&) override;
		void dump_scaling(const StaticString& name, Scaling) override;
		void dump_size(const StaticString& name, const SizeF&) override;
		void dump_sound(const StaticString& name, const Sound&) override;
		void dump_state(const StaticString& name, WidgetState) override;
		void dump_text(const StaticString& name, const StaticString&) override;
		void dump_texture(const StaticString& name, const SharedPtr<Texture2D>&) override;

	private:
		IonObject& _object;
	};
}

#endif
