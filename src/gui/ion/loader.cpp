#include "loader.h"

#include <Yttrium/ion/value.h>
#include <Yttrium/renderer/texture_cache.h>

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

} // namespace Gui

} // namespace Yttrium
