#include <yttrium/ion/node.h>

#include "document.h"

namespace Yttrium
{
	bool IonNode::exists() const
	{
		return _name.allocator() != &NoAllocator;
	}

	IonNode::IonNode(IonDocumentPrivate& document)
		: IonList(document)
		, _name(&document.allocator())
	{
	}

	IonNode::IonNode(IonDocumentPrivate& document, const StaticString& name)
		: IonList(document)
		, _name(name, &document.allocator())
	{
	}

	IonNode::IonNode(IonDocumentPrivate& document, const StaticString& name, const ByReference&)
		: IonList(document)
		, _name(name, ByReference(), &document.allocator())
	{
	}
}
