#include "node.h"

#include <Yttrium/ion/document.h>

namespace Yttrium
{

namespace Ion
{

const Node null_node;

Node::Node()
	: List(nullptr)
	, _name(static_cast<Allocator *>(nullptr))
{
}

void Node::serialize(String *result, int indentation) const
{
	if (indentation > 0)
	{
		result->append('\t', indentation);
	}
	result->append(_name);
	if (!is_empty())
	{
		List::serialize(result, indentation, true);
	}
}

String Node::serialize(int indentation, Allocator *allocator) const noexcept
{
	String result(allocator ? allocator : document()->allocator());

	serialize(&result, indentation);
	return result;
}

StaticString Node::string(const StaticString& default_value) const noexcept
{
	const Value *value = first();
	return (value && value->is_string()) ? value->string() : default_value;
}

Node::Node(Document *document, const StaticString &name)
	: List(document)
	, _name(name, document->_allocator)
{
}

Node::Node(Document *document, const StaticString &name, const ByReference &)
	: List(document)
	, _name(name, ByReference(), document->_allocator)
{
}

} // namespace Ion

} // namespace Yttrium
