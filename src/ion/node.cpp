#include "node.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/value.h>

namespace Yttrium
{
	namespace Ion
	{
		const IonNode null_node;
	}

	IonNode::IonNode()
		: IonList(nullptr)
		, _name(static_cast<Allocator*>(nullptr))
	{
	}

	void IonNode::serialize(String* result, int indentation) const
	{
		if (indentation > 0)
			result->append('\t', indentation);
		result->append(_name);
		if (!is_empty())
			IonList::serialize(result, indentation, true);
	}

	String IonNode::serialize(int indentation, Allocator* allocator) const
	{
		String result(allocator ? allocator : document()->allocator());
		serialize(&result, indentation);
		return std::move(result);
	}

	StaticString IonNode::string(const StaticString& default_value) const
	{
		const IonValue* value = first();
		return value && value->is_string() ? value->string() : default_value;
	}

	IonNode::IonNode(IonDocument* document, const StaticString& name)
		: IonList(document)
		, _name(name, document->allocator())
	{
	}

	IonNode::IonNode(IonDocument* document, const StaticString& name, const ByReference&)
		: IonList(document)
		, _name(name, ByReference(), document->allocator())
	{
	}
}
