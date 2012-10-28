#include "../logging.h"

#include "loader.h"

#include <Yttrium/ion/document.h>
#include <Yttrium/renderer/texture_cache.h>

#include "../manager.h"
#include "../scene.h"

#include "property_loader.h"

namespace Yttrium
{

namespace Gui
{

namespace
{

bool load_object(const Ion::List &source, const Ion::Object **object,
	const StaticString **name, Ion::List::ConstRange *classes)
{
	const Ion::Object *result_object = nullptr;
	const StaticString *result_name = nullptr;
	const Ion::List *result_classes = nullptr;

	for (Ion::List::ConstRange r = source.values(); !r.is_empty(); r.pop_first())
	{
		const Ion::Value &value = r.first();

		if (value.is_object())
		{
			if (result_object)
			{
				return false;
			}

			result_object = value.object();
		}
		else if (value.is_string())
		{
			if (result_name)
			{
				return false;
			}

			value.get(&result_name);
		}
		else if (value.is_list())
		{
			if (result_classes)
			{
				return false;
			}

			for (Ion::List::ConstRange s = value.list().values(); !s.is_empty(); s.pop_first())
			{
				if (!s.first().is_string())
				{
					return false;
				}
			}

			result_classes = &value.list();
		}
	}

	if (!result_object)
	{
		return false;
	}

	*object = result_object;
	*name = result_name;
	*classes = result_classes ? result_classes->values() : Ion::List::ConstRange();

	return true;
}

bool load_object(const Ion::List &source, const Ion::Object **object,
	const StaticString **name, const StaticString **class_name)
{
	const Ion::Object *result_object;
	const StaticString *result_name;
	Ion::List::ConstRange result_classes;

	if (!load_object(source, &result_object, &result_name, &result_classes))
	{
		return false;
	}

	// TODO: Make use of multiple classes.

	if (result_classes.size() > 1)
	{
		return false;
	}

	const StaticString *result_class = nullptr;

	if (!result_classes.is_empty())
	{
		result_classes.first().get(&result_class);
	}

	*object = result_object;
	*name = result_name;
	*class_name = result_class;

	return true;
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool IonLoader::load(const StaticString &source_name, bool is_internal)
{
	Ion::Document document(_manager->allocator());

	Y_LOG_DEBUG("[Gui.Manager] Loading \"" << source_name << "\"...");

	if (!document.load(source_name))
	{
		Y_LOG("[Gui.Manager] Can't load \"" << source_name << "\"...");
		return false;
	}

	if (!is_internal)
	{
		const Ion::Node *node;

		if (document.last(S("size"), &node))
		{
			Vector2f size;

			if (IonPropertyLoader::load_size(&size, *node))
			{
				_manager->set_size(size);
			}
		}

		if (document.last(S("scale"), &node))
		{
			Scaling scaling;

			if (IonPropertyLoader::load_scaling(&scaling, *node))
			{
				_manager->set_scaling(scaling);
			}
		}
	}

	load(document);

	Y_LOG_TRACE("[Gui.Manager] Loaded \"" << source_name << "\"");

	return true;
}

void IonLoader::load(const Ion::Object &source)
{
	for (Ion::Object::ConstRange r = source.nodes(); !r.is_empty(); r.pop_first())
	{
		const Ion::Node &node = r.first();

		if (node.name() == S("include"))
		{
			const StaticString *include_path;

			if (node.first(&include_path))
			{
				load(*include_path, true);
			}
		}
		else if (node.name() == S("class"))
		{
			const Ion::Object  *object;
			const StaticString *object_name;
			const StaticString *class_name;

			if (!load_object(node, &object, &object_name, &class_name))
			{
				continue;
			}

			Y_LOG_DEBUG("[Gui.Manager] Loading class \"" << *object_name << "\"...");

			if (!_classes.add(*object_name, *object, class_name))
			{
				Y_LOG("[Gui.Manager] Can' load class \"" << *object_name << "\"");
			}
		}
		else if (node.name() == S("scene"))
		{
			const Ion::Object  *object;
			const StaticString *object_name;
			const StaticString *class_name;

			if (!load_object(node, &object, &object_name, &class_name))
			{
				continue;
			}

			if (class_name && *class_name != S("root"))
			{
				Y_LOG("[Gui.Manager] Unknown scene \"" << *object_name << "\" option \"" << *class_name << "\" ignored");
				class_name = nullptr;
			}

			Y_LOG_DEBUG("[Gui.Manager] Loading scene \"" << *object_name << "\"...");

			Scene *scene = _manager->create_scene(*object_name);

			if (!scene)
			{
				continue;
			}

			if (!load_scene(scene, *object))
			{
				Y_LOG("[Gui.Manager] Can't load scene \"" << *object_name << "\"");
				_manager->delete_scene(scene);
				continue;
			}

			_manager->add_scene(scene, class_name != nullptr);
		}
		else if (node.name() == S("on_scene_change"))
		{
			Ion::List::ConstRange s = node.values();

			if (s.size() != 2 || !s.first().is_list() || !s.last().is_string())
			{
				Y_LOG("[Gui.Manager] Bad 'on_scene_change'");
				continue;
			}

			const StaticString *from;
			const StaticString *to;
			const StaticString *action;

			Ion::List::ConstRange t = s.first().list().values();

			if (t.size() != 2 || !t.first().get(&from) || !t.last().get(&to)
				|| !s.last().get(&action))
			{
				Y_LOG("[Gui.Manager] Bad 'on_scene_change'");
				continue;
			}

			_manager->set_scene_change_action(*from, *to, *action);
		}
		else if (node.name() == S("font"))
		{
			const Ion::Object  *object;
			const StaticString *object_name;
			const StaticString *class_name;

			if (!load_object(node, &object, &object_name, &class_name))
			{
				continue;
			}

			Y_LOG_DEBUG("[Gui.Manager] Loading font \"" << *object_name << "\"...");

			const StaticString *font_name;
			const StaticString *texture_name;

			if (!(IonPropertyLoader::load_text(&font_name, *object, "file")
				&& IonPropertyLoader::load_text(&texture_name, *object, "texture")))
			{
				continue;
			}

			_manager->set_font(*object_name, *font_name, *texture_name);
		}
	}
}

bool IonLoader::load_scene(Scene *scene, const Ion::Object &source) const
{
	scene->reserve(source.size());

	for (Ion::Object::ConstRange r = source.nodes(); !r.is_empty(); r.pop_first())
	{
		const Ion::Node &node = r.first();

		if (node.name() == S("size"))
		{
			Vector2f size;

			if (IonPropertyLoader::load_size(&size, node))
			{
				scene->set_size(size);
			}
		}
		else if (node.name() == S("scale"))
		{
			Scaling scaling;

			if (IonPropertyLoader::load_scaling(&scaling, node))
			{
				scene->set_scaling(scaling);
			}
		}
		if (node.name() == S("transparent"))
		{
			scene->set_transparent(true);
		}
		else if (node.name() == S("bind"))
		{
			Ion::Node::ConstRange s = node.values();

			if (s.size() == 2 && s.first().type() == Ion::Value::StringType
				&& s.last().type() == Ion::Value::StringType)
			{
				scene->bind(s.first().string(), s.last().string());
			}
		}
		else
		{
			// TODO: Implement properly.

			IonPropertyLoader loader;

			scene->load_widget(node.name(), StaticString(), loader);
		}
	}

	return true; // NOTE: Useless result.
}

} // namespace Gui

} // namespace Yttrium
