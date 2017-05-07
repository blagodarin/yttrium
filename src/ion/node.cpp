#include <yttrium/ion/node.h>

#include "document.h"

namespace Yttrium
{
	bool IonNode::exists() const
	{
		return document().exists();
	}

	IonNode::IonNode(IonDocumentImpl& document)
		: IonList{document}
	{
	}

	IonNode::IonNode(IonDocumentImpl& document, std::string_view name)
		: IonList{document}
		, _name{name}
	{
	}

	IonNode::IonNode(IonDocumentImpl& document, std::string_view name, const ByReference&)
		: IonList{document}
		, _name{name, ByReference{}}
	{
	}
}
