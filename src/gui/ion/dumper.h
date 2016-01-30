#ifndef _src_gui_ion_dumper_h_
#define _src_gui_ion_dumper_h_

#include <yttrium/static_string.h>

namespace Yttrium
{
	class GuiImpl;
	class GuiLayer;
	class IonNode;

	class GuiIonDumper
	{
	public:
		GuiIonDumper(const GuiImpl& gui) : _gui(gui) {}

		void dump(const StaticString& filename) const;

	private:
		void dump_layer(const GuiLayer& layer, IonNode* node) const;

	private:
		const GuiImpl& _gui;
	};
}

#endif
