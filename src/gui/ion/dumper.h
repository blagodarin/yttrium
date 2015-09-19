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

		static void dump_scene(const GuiScene& scene, IonNode* node);

	private:

		const GuiImpl& _gui;
	};
}

#endif
