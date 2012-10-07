#include "loader.h"

#include <Yttrium/ion/object.h>
#include <Yttrium/ion/value.h>
#include <Yttrium/renderer/texture_cache.h>

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
	*classes = result_classes->values();

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

bool IonLoader::load_image(Image *image, const Ion::Node &node) const
{
	const Ion::Object *object;
	const StaticString *name;
	const StaticString *class_name;

	if (!load_object(node, &object, &name, &class_name))
	{
		return false;
	}

	// TODO: Implement.

	return false;
}

bool IonLoader::load_scene(Scene *scene, const Ion::Object &source) const
{
	bool result = false;

	scene->reserve(source.size());

	for (Ion::Object::ConstRange r = source.nodes(); !r.is_empty(); r.pop_first())
	{
		const Ion::Node &node = r.first();

		if (node.name() == "root")
		{
			result = true; // NOTE: The last scene becomes a root one. Is it OK?
		}
		else if (node.name() == "size")
		{
			Vector2f size;

			if (IonPropertyLoader::load_size(&size, node))
			{
				scene->set_size(size);
			}
		}
		else if (node.name() == "scale")
		{
			Scaling scaling;

			if (IonPropertyLoader::load_scaling(&scaling, node))
			{
				scene->set_scaling(scaling);
			}
		}
		if (node.name() == "transparent")
		{
			scene->set_transparent(true);
		}
		else if (node.name() == "bind")
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

			scene->add_widget(node.name(), StaticString(), loader);
		}
	}
	return result;
}

} // namespace Gui

} // namespace Yttrium
