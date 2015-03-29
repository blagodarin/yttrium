#ifndef __GUI_ION_DUMPER_H
#define __GUI_ION_DUMPER_H

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

#endif // __GUI_ION_DUMPER_H
