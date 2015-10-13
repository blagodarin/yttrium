#ifndef _src_gui_ion_loader_h_
#define _src_gui_ion_loader_h_

#include <yttrium/string.h>
#include "classes.h"

namespace Yttrium
{
	class GuiImpl;
	class GuiScene;

	class GuiIonLoader
	{
	public:

		GuiIonLoader(GuiImpl& gui);

		bool load(const StaticString& source_name, bool is_internal = false);

	private:

		void load(const IonObject& source);
		void load_scene(GuiScene& scene, const IonObject& source) const;

	private:

		GuiImpl& _gui;
		GuiClasses _classes;
		bool _has_default_font = false;
		String _default_font_name;
	};
}

#endif
