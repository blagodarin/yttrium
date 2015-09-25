#ifndef _src_gui_ion_dumper_h_
#define _src_gui_ion_dumper_h_

#include <yttrium/static_string.h>

namespace Yttrium
{
	class GuiImpl;
	class GuiScene;
	class IonNode;

	class GuiIonDumper
	{
	public:

		GuiIonDumper(const GuiImpl& gui): _gui(gui) {}

		void dump(const StaticString& filename) const;

	private:

		void dump_scene(const GuiScene& scene, IonNode* node) const;

	private:

		const GuiImpl& _gui;
	};
}

#endif
