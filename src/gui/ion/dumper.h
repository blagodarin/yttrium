#ifndef __GUI_ION_DUMPER_H
#define __GUI_ION_DUMPER_H

#include <Yttrium/static_string.h>

namespace Yttrium
{

namespace Ion
{

class Node;

} // namespace Ion

namespace Gui
{

class ManagerImpl;
class Scene;

class IonDumper
{
public:

	inline IonDumper(const ManagerImpl *manager);

public:

	void dump(const StaticString &filename) const;

private:

	static void dump_scene(const Scene *scene, Ion::Node *node);

private:

	const ManagerImpl *_manager;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IonDumper::IonDumper(const ManagerImpl *manager)
	: _manager(manager)
{
}

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_ION_DUMPER_H
