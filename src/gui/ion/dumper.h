#ifndef __GUI_ION_DUMPER_H
#define __GUI_ION_DUMPER_H

#include <yttrium/static_string.h>

namespace Yttrium
{

namespace Ion
{

class Node;

} // namespace Ion

class GuiImpl;
class GuiScene;

class GuiIonDumper
{
public:

	GuiIonDumper(const GuiImpl* gui)
		: _gui(gui)
	{
	}

public:

	void dump(const StaticString& filename) const;

private:

	static void dump_scene(const GuiScene* scene, Ion::Node* node);

private:

	const GuiImpl* _gui;
};

} // namespace Yttrium

#endif // __GUI_ION_DUMPER_H
