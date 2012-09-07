#include <Yttrium/ion/node.h>

#include <Yttrium/ion/document.h>

namespace Yttrium
{

namespace Ion
{

void Node::to_string(String *result, int indentation) const
{
	if (indentation > 0)
	{
		result->append('\t', indentation);
	}
	result->append(_name);
	if (!is_empty())
	{
		List::to_string(result, indentation, true);
	}
}

String Node::to_string(int indentation, Allocator *allocator) const noexcept
{
	String result(allocator ? allocator : _name.allocator()); // NOTE: Wrong allocator?

	to_string(&result, indentation);
	return result;
}

Node::Node(Document *document, const StaticString &name)
	: List(document)
	, _name(name, document->_allocator)
	, _next(nullptr)
{
}

Node::Node(Document *document, const StaticString &name, const ByReference &)
	: List(document)
	, _name(name, ByReference(), document->_allocator)
	, _next(nullptr)
{
}

} // namespace Ion

} // namespace Yttrium
