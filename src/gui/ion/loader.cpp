#include "loader.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/value.h>
#include <yttrium/log.h>
#include <yttrium/texture_cache.h>
#include "../gui.h"
#include "../scene.h"
#include "property_loader.h"

namespace Yttrium
{

namespace
{

bool load_object(const IonList &source, const IonObject **object,
	const StaticString **name, IonList::ConstRange *classes)
{
	const IonObject *result_object = nullptr;
	const StaticString *result_name = nullptr;
	const IonList *result_classes = nullptr;

	for (const IonValue &value: source)
	{
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

			for (const IonValue &class_name: value.list())
			{
				if (!class_name.is_string())
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
	*classes = result_classes ? result_classes->values() : IonList::ConstRange();

	return true;
}

bool load_object(const IonList &source, const IonObject **object,
	const StaticString **name, const StaticString **class_name)
{
	const IonObject *result_object;
	const StaticString *result_name;
	IonList::ConstRange result_classes;

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
		result_classes->get(&result_class);
	}

	*object = result_object;
	*name = result_name;
	*class_name = result_class;

	return true;
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GuiIonLoader::GuiIonLoader(GuiImpl *gui)
	: _gui(gui)
	, _classes(_gui->internal_allocator())
{
}

bool GuiIonLoader::load(const StaticString &source_name, bool is_internal)
{
	IonDocument document(_gui->internal_allocator());

	if (!document.load(source_name))
	{
		Y_LOG("[Gui] Can't load \"" << source_name << "\"...");
		return false;
	}

	if (!is_internal)
	{
		const IonNode *node;

		if (document.last(S("size"), &node))
		{
			Vector2f size;

			if (GuiIonPropertyLoader::load_size(&size, *node))
			{
				_gui->set_size(size);
			}
		}

		if (document.last(S("scale"), &node))
		{
			Scaling scaling;

			if (GuiIonPropertyLoader::load_scaling(&scaling, *node))
			{
				_gui->set_scaling(scaling);
			}
		}
	}

	load(document);

	return true;
}

void GuiIonLoader::load(const IonObject &source)
{
	for (const IonNode &node: source.nodes())
	{
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
			const IonObject  *object;
			const StaticString *object_name;
			const StaticString *class_name;

			if (!load_object(node, &object, &object_name, &class_name))
			{
				continue;
			}

			if (!_classes.add(*object_name, *object, class_name))
			{
				Y_LOG("[Gui] Can' load class \"" << *object_name << "\"");
			}
		}
		else if (node.name() == S("scene"))
		{
			const IonObject  *object;
			const StaticString *object_name;
			const StaticString *class_name;

			if (!load_object(node, &object, &object_name, &class_name))
			{
				continue;
			}

			if (class_name && *class_name != S("root"))
			{
				Y_LOG("[Gui] Unknown scene \"" << *object_name << "\" option \"" << *class_name << "\" ignored");
				class_name = nullptr;
			}

			GuiScene *scene = _gui->create_scene(*object_name);

			if (scene)
			{
				load_scene(scene, *object);
				_gui->add_scene(scene, class_name != nullptr);
			}
		}
		else if (node.name() == S("on_scene_change"))
		{
			IonList::ConstRange s = node.values();

			if (s.size() != 2 || !s->is_list() || !s.last().is_string())
			{
				Y_LOG("[Gui] Bad 'on_scene_change'");
				continue;
			}

			const StaticString *from;
			const StaticString *to;
			const StaticString *action;

			IonList::ConstRange t = s->list().values();

			if (t.size() != 2 || !t->get(&from) || !t.last().get(&to)
				|| !s.last().get(&action))
			{
				Y_LOG("[Gui] Bad 'on_scene_change'");
				continue;
			}

			_gui->set_scene_change_action(
				String(*from, _gui->internal_allocator()),
				String(*to, _gui->internal_allocator()),
				String(*action, _gui->internal_allocator()));
		}
		else if (node.name() == S("font"))
		{
			const IonObject  *object;
			const StaticString *object_name;
			const StaticString *class_name;

			if (!load_object(node, &object, &object_name, &class_name))
			{
				continue;
			}

			const StaticString *font_name;
			const StaticString *texture_name;

			if (!(GuiIonPropertyLoader::load_text(&font_name, *object, "file")
				&& GuiIonPropertyLoader::load_text(&texture_name, *object, "texture")))
			{
				continue;
			}

			_gui->set_font(*object_name, *font_name, *texture_name);
		}
	}
}

void GuiIonLoader::load_scene(GuiScene *scene, const IonObject &source) const
{
	scene->reserve(source.size());

	for (const IonNode &node: source.nodes())
	{
		if (node.name() == S("size"))
		{
			Vector2f size;

			if (GuiIonPropertyLoader::load_size(&size, node))
			{
				scene->set_size(size);
			}
		}
		else if (node.name() == S("scale"))
		{
			Scaling scaling;

			if (GuiIonPropertyLoader::load_scaling(&scaling, node))
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
			IonNode::ConstRange s = node.values();

			if (s.size() == 2 && s->type() == IonValue::StringType
				&& s.last().type() == IonValue::StringType)
			{
				scene->bind(s->string(), s.last().string());
			}
		}
		else
		{
			const IonObject  *object;
			const StaticString *object_name;
			const StaticString *class_name;

			if (!load_object(node, &object, &object_name, &class_name))
			{
				continue;
			}

			GuiIonPropertyLoader loader(object, (class_name ? _classes.find(*class_name) : nullptr), _gui);

			scene->load_widget(node.name(), (object_name ? *object_name : StaticString()), loader);
		}
	}
}

} // namespace Yttrium
