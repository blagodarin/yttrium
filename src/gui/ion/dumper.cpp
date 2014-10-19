#include "dumper.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>

#include "../gui.h"
#include "../scene.h"

namespace Yttrium
{

void GuiIonDumper::dump(const StaticString& filename) const
{
	Ion::Document document(_gui.internal_allocator());

	if (_gui._has_size)
	{
		Ion::Node* size_node = document.append("size");

		size_node->append(String::from_dec(_gui._size.x));
		size_node->append(String::from_dec(_gui._size.y));
	}

	Ion::Node* scale_node = document.append("scale");

	switch (_gui._scaling)
	{
	case Scaling::Stretch: scale_node->append("stretch"); break;
	case Scaling::Min:     scale_node->append("min");     break;
	case Scaling::Max:     scale_node->append("max");     break;
	case Scaling::Fit:     scale_node->append("fit");     break;
	}

	// TODO: Dump fonts.

	for (const auto& scene: _gui._scenes)
	{
		Ion::Node* scene_node = document.append("scene");

		scene_node->append(scene.first);

		if (scene.first == _gui._scene_stack.front()->name())
			scene_node->append_list()->append("root");

		dump_scene(*scene.second, scene_node);
	}

	for (const auto& action: _gui._scene_actions)
	{
		Ion::Node* on_scene_change_node = document.append("on_scene_change");

		Ion::List* scene_list = on_scene_change_node->append_list();

		scene_list->append(action.first.first);
		scene_list->append(action.first.second);

		on_scene_change_node->append(action.second);
	}

	document.save(filename);
}

void GuiIonDumper::dump_scene(const GuiScene& scene, Ion::Node* scene_node)
{
	Ion::Object* scene_object = scene_node->append_object();

	Ion::Node* size_node = scene_object->append("size");

	size_node->append(String::from_dec(scene._size.x));
	size_node->append(String::from_dec(scene._size.y));

	Ion::Node* scale_node = scene_object->append("scale");

	switch (scene._scaling)
	{
	case Scaling::Stretch: scale_node->append("stretch"); break;
	case Scaling::Min:     scale_node->append("min");     break;
	case Scaling::Max:     scale_node->append("max");     break;
	case Scaling::Fit:     scale_node->append("fit");     break;
	}

	if (scene._is_transparent)
		scene_object->append("transparent");

	for (const auto& binding: scene._bindings.map())
	{
		Ion::Node* bind_node = scene_object->append("bind");

		bind_node->append(binding.first);
		bind_node->append(binding.second);
	}

	// TODO: Dump widgets.
}

} // namespace Yttrium
