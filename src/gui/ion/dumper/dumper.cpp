#include "dumper.h"

#include <Yttrium/ion/document.h>

#include "../../manager.h"
#include "../../scene.h"

namespace Yttrium
{

namespace Gui
{

void IonDumper::dump(const StaticString &filename) const
{
	Ion::Document document(_manager->allocator());

	if (_manager->_has_size)
	{
		Ion::Node *size_node = document.append("size");

		size_node->append(String::from_dec(_manager->_size.x));
		size_node->append(String::from_dec(_manager->_size.y));
	}

	Ion::Node *scale_node = document.append("scale");

	switch (_manager->_scaling)
	{
	case Scaling::Stretch: scale_node->append("stretch"); break;
	case Scaling::Min:     scale_node->append("min");     break;
	case Scaling::Max:     scale_node->append("max");     break;
	case Scaling::Fit:     scale_node->append("fit");     break;
	}

	// TODO: Dump fonts.

	for (ManagerImpl::Scenes::const_iterator i = _manager->_scenes.begin(); i != _manager->_scenes.end(); ++i)
	{
		Ion::Node *scene_node = document.append("scene");

		scene_node->append(i->first);

		if (i->first == _manager->_scene_stack.front()->name())
		{
			scene_node->append_list()->append("root");
		}

		dump_scene(i->second, scene_node);
	}

	for (ManagerImpl::SceneActions::const_iterator i = _manager->_scene_actions.begin(); i != _manager->_scene_actions.end(); ++i)
	{
		Ion::Node *on_scene_change_node = document.append("on_scene_change");

		Ion::List *scene_list = on_scene_change_node->append_list();

		scene_list->append(i->first.first);
		scene_list->append(i->first.second);

		on_scene_change_node->append(i->second);
	}

	document.save(filename);
}

void IonDumper::dump_scene(const Scene *scene, Ion::Node *scene_node)
{
	Ion::Object *scene_object = scene_node->append_object();

	Ion::Node *size_node = scene_object->append("size");

	size_node->append(String::from_dec(scene->_size.x));
	size_node->append(String::from_dec(scene->_size.y));

	Ion::Node *scale_node = scene_object->append("scale");

	switch (scene->_scaling)
	{
	case Scaling::Stretch: scale_node->append("stretch"); break;
	case Scaling::Min:     scale_node->append("min");     break;
	case Scaling::Max:     scale_node->append("max");     break;
	case Scaling::Fit:     scale_node->append("fit");     break;
	}

	if (scene->_is_transparent)
	{
		scene_object->append("transparent");
	}

	const Bindings::Map &bindings = scene->_bindings.map();

	for (Bindings::Map::const_iterator i = bindings.begin(); i != bindings.end(); ++i)
	{
		Ion::Node *bind_node = scene_object->append("bind");

		bind_node->append(i->first);
		bind_node->append(i->second);
	}

	// TODO: Dump widgets.
}

} // namespace Gui

} // namespace Yttrium
