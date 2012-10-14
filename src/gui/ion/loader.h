#ifndef __GUI_ION_LOADER_H
#define __GUI_ION_LOADER_H

#include "classes.h"

namespace Yttrium
{

namespace Gui
{

class ManagerImpl;
class Scene;

class IonLoader
{
public:

	inline IonLoader(ManagerImpl *manager);

public:

	bool load(const StaticString &source_name, bool is_internal = false);

private:

	void load(const Ion::Object &source);

	bool load_scene(Scene *scene, const Ion::Object &source) const;

private:

	ManagerImpl *_manager;
	Classes      _classes;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IonLoader::IonLoader(ManagerImpl *manager)
	: _manager(manager)
{
}

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_ION_LOADER_H
