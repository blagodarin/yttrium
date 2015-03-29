#include <yttrium/ion/utils.h>

#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/vector.h>

namespace Yttrium
{
	namespace Ion
	{
		bool read(const IonValue& source, Vector2f& value)
		{
			const IonList* list;
			if (!source.get(&list) || list->size() != 2)
				return false;
			if (!list->first()->string().to_number(&value.x))
				return false;
			if (!list->last()->string().to_number(&value.y))
				return false;
			return true;
		}

		bool read(const IonObject& source, const StaticString& name, float& value)
		{
			const StaticString* value_text;
			if (!source.last(name, &value_text))
				return false;

			if (!value_text->to_number(&value))
				return false;

			return true;
		}

		bool read(const IonObject& source, const StaticString& name, const IonObject*& value)
		{
			const IonNode& node = source.last(name);
			if (node.is_empty() || !node.last()->is_object())
				return false;

			value = node.last()->object();
			return true;
		}
	}
}
