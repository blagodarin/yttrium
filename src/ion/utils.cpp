#include <yttrium/ion/utils.h>

#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>

namespace
{
	void append(Yttrium::IonList& target, const Yttrium::IonValue& source)
	{
		switch (source.type())
		{
		case Yttrium::IonValue::Type::List:
			{
				const auto target_list = target.append_list();
				for (const auto& source_value : source.list())
					append(*target_list, source_value);
			}
			break;
		case Yttrium::IonValue::Type::Object:
			{
				const auto target_object = target.append_object();
				for (const auto& source_node : *source.object())
					Yttrium::Ion::append(*target_object, source_node);
			}
			break;
		default:
			target.append(source.string());
			break;
		}
	}
}

namespace Yttrium
{
	namespace Ion
	{
		void append(IonObject& target, const IonNode& source)
		{
			const auto target_node = target.append(source.name());
			for (const auto& source_value : source)
				::append(*target_node, source_value);
		}

		bool get(const IonNode& source, std::string_view& value)
		{
			return !source.is_empty() && source.last()->get(value);
		}
	}
}
