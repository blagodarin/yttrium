#ifndef __GUI_ION_LOADER_H
#define __GUI_ION_LOADER_H

#include <Yttrium/ion/node.h>
#include <Yttrium/renderer/texture.h>

namespace Yttrium
{

namespace Gui
{

class Image;
class Scene;

class IonLoader
{
public:

	bool load_image(Image *image, const Ion::Node &node) const;

private:

	bool load_scene(Scene *scene, const Ion::Object &source) const;
};

} // namespace Gui

} // namespace Yttrium

#endif // __GUI_ION_LOADER_H
