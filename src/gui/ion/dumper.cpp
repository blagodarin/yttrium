#include "dumper.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/string_format.h>
#include "../gui.h"
#include "../scene.h"

namespace Yttrium
{
	void GuiIonDumper::dump(const StaticString& filename) const
	{
		IonDocument document(_gui.internal_allocator());

		if (_gui._has_size)
		{
			IonNode* size_node = document.root().append("size"_s);
			size_node->append(String(_gui.internal_allocator()) << _gui._size.x);
			size_node->append(String(_gui.internal_allocator()) << _gui._size.y);
		}

		IonNode* scale_node = document.root().append("scale"_s);

		switch (_gui._scaling)
		{
		case Scaling::Stretch: scale_node->append("stretch"_s); break;
		case Scaling::Min:     scale_node->append("min"_s);     break;
		case Scaling::Max:     scale_node->append("max"_s);     break;
		case Scaling::Fit:     scale_node->append("fit"_s);     break;
		}

		// TODO: Dump fonts.

		for (const auto& scene : _gui._scenes)
		{
			IonNode* scene_node = document.root().append("scene"_s);

			scene_node->append(scene.first);

			if (scene.first == _gui._scene_stack.front()->name())
				scene_node->append_list()->append("root"_s);
			else if (scene.second->is_transparent())
				scene_node->append_list()->append("transparent"_s);

			dump_scene(*scene.second, scene_node);
		}

		for (const auto& action : _gui._scene_actions)
		{
			IonNode* on_scene_change_node = document.root().append("on_scene_change"_s);

			IonList* scene_list = on_scene_change_node->append_list();

			scene_list->append(action.first.first);
			scene_list->append(action.first.second);

			on_scene_change_node->append(action.second.first);
		}

		document.save(filename);
	}

	void GuiIonDumper::dump_scene(const GuiScene& scene, IonNode* scene_node) const
	{
		IonObject* scene_object = scene_node->append_object();

		IonNode* size_node = scene_object->append("size"_s);

		size_node->append(String(_gui.internal_allocator()) << scene._size.x);
		size_node->append(String(_gui.internal_allocator()) << scene._size.y);

		IonNode* scale_node = scene_object->append("scale"_s);

		switch (scene._scaling)
		{
		case Scaling::Stretch: scale_node->append("stretch"_s); break;
		case Scaling::Min:     scale_node->append("min"_s);     break;
		case Scaling::Max:     scale_node->append("max"_s);     break;
		case Scaling::Fit:     scale_node->append("fit"_s);     break;
		}

		for (const auto& binding : scene._bindings)
		{
			const auto bound_node = scene_object->append("bind"_s);
			bound_node->append(binding.first);
			bound_node->append(binding.second);
		}

		// TODO: Dump widgets.
	}
}
