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

Node::Node(Document *document, const StaticString &name)
	: List(document)
	, _name(name, document->_allocator)
	, _next(nullptr)
{
}

Node::Node(Document *document, const StaticString &name, const String::Reference &)
	: List(document)
	, _name(name, String::Ref, document->_allocator)
	, _next(nullptr)
{
}

} // namespace Ion

} // namespace Yttrium
