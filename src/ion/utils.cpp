#include "utils.h"

#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/value.h>
#include <yttrium/math/point.h>
#include <yttrium/string_utils.h>

namespace
{
	using namespace Yttrium;

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

	void serialize_list(std::string&, const IonList&, bool is_node, int indentation);
	void serialize_object(std::string&, const IonObject&, bool is_root, int indentation);

	void serialize_value(std::string& result, const IonValue& value, int indentation)
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
			append_to(result, '"', value.string().escaped("\\\"", '\\'), '"');
			break;
		}
	}

	void serialize_list(std::string& result, const IonList& list, bool is_node, int indentation)
	{
		if (!is_node)
			append_to(result, '[');
		auto value = list.begin();
		if (value != list.end())
		{
			if (is_node && indentation >= 0 && value->type() != IonValue::Type::Object)
				append_to(result, ' ');
			for (;;)
			{
				serialize_value(result, *value, indentation);
				++value;
				if (value == list.end())
					break;
				if (indentation >= 0 && value->type() != IonValue::Type::Object)
					append_to(result, ' ');
			}
		}
		if (!is_node)
			append_to(result, ']');
	}

	void serialize_node(std::string& result, const IonNode& node, int indentation)
	{
		if (indentation > 0)
			append_to(result, Repeat{ '\t', indentation });
		append_to(result, node.name());
		if (!node.is_empty())
			serialize_list(result, node, true, indentation);
	}

	void serialize_object(std::string& result, const IonObject& object, bool is_root, int indentation)
	{
		if (indentation < 0)
		{
			if (!is_root)
				append_to(result, '{');
			bool need_separator = false;
			for (const auto& node : object)
			{
				if (need_separator)
					append_to(result, ' ');
				serialize_node(result, node, indentation);
				need_separator = node.is_empty();
			}
			if (!is_root)
				append_to(result, '}');
		}
		else
		{
			if (!is_root)
				append_to(result, '\n', Repeat{ '\t', indentation }, "{\n"_s);
			const auto node_indentation = indentation + !is_root;
			for (const auto& node : object)
			{
				serialize_node(result, node, node_indentation);
				append_to(result, '\n');
			}
			if (!is_root)
				append_to(result, Repeat{ '\t', indentation }, '}');
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

		bool get(const IonValue& source, PointF& value)
		{
			if (source.type() != IonValue::Type::List)
				return false;
			const auto& list = source.list();
			return list.size() == 2
				&& list.first()->string().to_number(value.data()[0])
				&& list.last()->string().to_number(value.data()[1]);
		}

		bool get(const IonNode& source, const StaticString*& value)
		{
			return !source.is_empty() && source.last()->get(&value);
		}

		bool get(const IonObject& source, const StaticString& name, float& value)
		{
			const auto& node = source.last(name);
			const StaticString* value_string;
			return !node.is_empty() && node.last()->get(&value_string) && value_string->to_number(value);
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

		std::string serialize(const IonObject& object, bool root, int indentation)
		{
			std::string result;
			::serialize_object(result, object, root, indentation);
			return result;
		}

		StaticString to_string(const IonObject& source, const StaticString& name)
		{
			const auto* value = source.first(name).first();
			return value && value->type() == IonValue::Type::String ? value->string() : StaticString();
		}
	}
}
