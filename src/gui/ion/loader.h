#ifndef _src_gui_ion_loader_h_
#define _src_gui_ion_loader_h_

#include <yttrium/string.h>
#include "classes.h"

namespace Yttrium
{
	class GuiImpl;
	class GuiLayer;
	class GuiLayout;
	class IonNode;

	class GuiIonLoader
	{
	public:
		GuiIonLoader(GuiImpl& gui);

		bool load(const StaticString& source_name);

	private:
		void load(const IonObject&);

		void load_class(const IonNode&, unsigned attributes);
		void load_font(const IonNode&, unsigned attributes);
		void load_include(const IonNode&, unsigned attributes);
		void load_layer(const IonNode&, unsigned attributes);

		void load_layer_layout(GuiLayer&, const IonNode&, int extra) const;
		void load_layer_on_key(GuiLayer&, const IonNode&, int extra) const;
		void load_layer_on_pop(GuiLayer&, const IonNode&, int extra) const;
		void load_layer_on_push(GuiLayer&, const IonNode&, int extra) const;

		void load_layout_size(GuiLayout&, const IonNode&) const;

	private:
		GuiImpl& _gui;
		Allocator& _allocator;
		GuiClasses _classes;
		bool _has_default_font = false;
		String _default_font_name;
	};
}

#endif
