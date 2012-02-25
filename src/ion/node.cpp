#include <Yttrium/ion/node.hpp>

#include <Yttrium/ion/document.hpp>

namespace Yttrium
{

namespace Ion
{

void Node::to_string(String *result, int indentation) const noexcept
{
	if (indentation > 0)
	{
		result->append('\t', indentation);
	}
	result->append(_name);
	if (!empty())
	{
		if (indentation >= 0)
		{
			result->append(' ');
		}
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
