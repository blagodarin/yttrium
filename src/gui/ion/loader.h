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

public:

	bool load(const StaticString& source_name, bool is_internal = false);

private:

	void load(const Ion::Object& source);
	void load_scene(GuiScene* scene, const Ion::Object& source) const;

private:

	GuiImpl*   _gui;
	GuiClasses _classes;
};

} // namespace Yttrium

#endif // __GUI_ION_LOADER_H
