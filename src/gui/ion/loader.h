#ifndef __GUI_ION_LOADER_H
#define __GUI_ION_LOADER_H

#include "classes.h"

namespace Yttrium
{
	class GuiImpl;
	class GuiScene;

	class GuiIonLoader
	{
	public:

		GuiIonLoader(GuiImpl* gui);

		bool load(const StaticString& source_name, bool is_internal = false);

	private:

		void load(const IonObject& source);
		void load_scene(GuiScene& scene, const IonObject& source) const;

	private:

		GuiImpl*   _gui;
		GuiClasses _classes;
	};
}

#endif // __GUI_ION_LOADER_H
