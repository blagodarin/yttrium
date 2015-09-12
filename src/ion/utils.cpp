#include "utils.h"

#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/vector.h>

namespace Yttrium
{
	namespace Ion
	{
		namespace
		{
			void append(IonList& target, const IonValue& source)
			{
				switch (source.type())
				{
				case IonValue::Type::List:
					{
						const auto target_list = target.append_list();
						for (const auto& source_value : source.list())
							append(*target_list, source_value);
					}
					break;
				case IonValue::Type::Object:
					{
						const auto target_object = target.append_object();
						for (const auto& source_node : *source.object())
							Ion::append(*target_object, source_node);
					}
					break;
				default:
					target.append(source.string());
					break;
				}
			}
		}

		void append(IonObject& target, const IonNode& source)
		{
			const auto target_node = target.append(source.name());
			for (const auto& source_value : source)
				append(*target_node, source_value);
		}

		bool get(const IonValue& source, Vector2& value)
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

		bool get(const IonNode& source, const StaticString*& value)
		{
			return !source.is_empty() && source.last()->get(&value);
		}

		bool get(const IonObject& source, const StaticString& name, float& value)
		{
			const auto& node = source.last(name);
			return !node.is_empty() && node.last()->get(&value);
		}

		bool get(const IonObject& source, const StaticString& name, const StaticString*& value)
		{
			const auto& node = source.last(name);
			return !node.is_empty() && node.last()->get(&value);
		}

		bool get(const IonObject& source, const StaticString& name, const IonObject*& value)
		{
			const auto& node = source.last(name);
			return !node.is_empty() && node.last()->get(&value);
		}

		namespace
		{
			void serialize_list(String&, const IonList&, bool is_node, int indentation);
			void serialize_object(String&, const IonObject&, bool is_root, int indentation);

			void serialize_value(String& result, const IonValue& value, int indentation)
			{
				switch (value.type())
				{
				case IonValue::Type::List:
					serialize_list(result, value.list(), false, indentation + (indentation >= 0));
					break;
				case IonValue::Type::Object:
					serialize_object(result, *value.object(), false, indentation);
					break;
				default:
					result.append('"').append(value.string().escaped("\\\"", '\\', result.allocator())).append('"');
					break;
				}
			}

			void serialize_list(String& result, const IonList& list, bool is_node, int indentation)
			{
				if (!is_node)
					result.append('[');
				auto value = list.begin();
				if (value != list.end())
				{
					if (is_node && indentation >= 0 && value->type() != IonValue::Type::Object)
						result.append(' ');
					for (;;)
					{
						serialize_value(result, *value, indentation);
						++value;
						if (value == list.end())
							break;
						if (indentation >= 0 && value->type() != IonValue::Type::Object)
							result.append(' ');
					}
				}
				if (!is_node)
					result.append(']');
			}

			void serialize_node(String& result, const IonNode& node, int indentation)
			{
				if (indentation > 0)
					result.append('\t', indentation);
				result.append(node.name());
				if (!node.is_empty())
					serialize_list(result, node, true, indentation);
			}

			void serialize_object(String& result, const IonObject& object, bool is_root, int indentation)
			{
				if (indentation < 0)
				{
					if (!is_root)
						result.append('{');
					bool need_separator = false;
					for (const auto& node : object)
					{
						if (need_separator)
							result.append(' ');
						serialize_node(result, node, indentation);
						need_separator = node.is_empty();
					}
					if (!is_root)
						result.append('}');
				}
				else
				{
					if (!is_root)
						result.append('\n').append('\t', indentation).append(S("{\n"));
					const auto node_indentation = indentation + !is_root;
					for (const auto& node : object)
					{
						serialize_node(result, node, node_indentation);
						result.append('\n');
					}
					if (!is_root)
						result.append('\t', indentation).append('}');
				}
			}
		}

		String serialize(const IonObject& object, bool root, int indentation, Allocator* allocator)
		{
			String result(allocator);
			serialize_object(result, object, root, indentation);
			return result;
		}

		StaticString to_string(const IonObject& source, const StaticString& name)
		{
			const auto* value = source.first(name).first();
			return value && value->type() == IonValue::Type::String ? value->string() : StaticString();
		}
	}
}
