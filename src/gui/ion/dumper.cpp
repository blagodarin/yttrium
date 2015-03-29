#include "dumper.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>

#include "../gui.h"
#include "../scene.h"

namespace Yttrium
{
	void GuiIonDumper::dump(const StaticString& filename) const
	{
		IonDocument document(_gui.internal_allocator());

		if (_gui._has_size)
		{
			IonNode* size_node = document.append("size");
			size_node->append(String::from_dec(_gui._size.x));
			size_node->append(String::from_dec(_gui._size.y));
		}

		IonNode* scale_node = document.append("scale");

		switch (_gui._scaling)
		{
		case Scaling::Stretch: scale_node->append("stretch"); break;
		case Scaling::Min:     scale_node->append("min");     break;
		case Scaling::Max:     scale_node->append("max");     break;
		case Scaling::Fit:     scale_node->append("fit");     break;
		}

		// TODO: Dump fonts.

		for (const auto& scene : _gui._scenes)
		{
			IonNode* scene_node = document.append("scene");

			scene_node->append(scene.first);

			if (scene.first == _gui._scene_stack.front()->name())
				scene_node->append_list()->append("root");

			dump_scene(*scene.second, scene_node);
		}

		for (const auto& action: _gui._scene_actions)
		{
			IonNode* on_scene_change_node = document.append("on_scene_change");

			IonList* scene_list = on_scene_change_node->append_list();

			scene_list->append(action.first.first);
			scene_list->append(action.first.second);

			on_scene_change_node->append(action.second.first);
		}

		document.save(filename);
	}

	void GuiIonDumper::dump_scene(const GuiScene& scene, IonNode* scene_node)
	{
		IonObject* scene_object = scene_node->append_object();

		IonNode* size_node = scene_object->append("size");

		size_node->append(String::from_dec(scene._size.x));
		size_node->append(String::from_dec(scene._size.y));

		IonNode* scale_node = scene_object->append("scale");

		switch (scene._scaling)
		{
		case Scaling::Stretch: scale_node->append("stretch"); break;
		case Scaling::Min:     scale_node->append("min");     break;
		case Scaling::Max:     scale_node->append("max");     break;
		case Scaling::Fit:     scale_node->append("fit");     break;
		}

		if (scene._is_transparent)
			scene_object->append("transparent");

		for (const auto& binding : scene._bindings.map())
		{
			const auto bound_node = scene_object->append("bind");
			bound_node->append(binding.first);
			bound_node->append(binding.second);
		}

		// TODO: Dump widgets.
	}
}
