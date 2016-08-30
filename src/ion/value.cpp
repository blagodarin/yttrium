#include <yttrium/ion/value.h>

#include "document.h"

namespace Yttrium
{
	IonValue::IonValue(IonDocumentPrivate& document)
		: _type(Type::List)
		, _string(&document.allocator())
		, _list(document)
	{
	}

	IonValue::IonValue(IonDocumentPrivate& document, const StaticString& string)
		: _type(Type::String)
		, _string(string, &document.allocator())
		, _list(document)
	{
	}

	IonValue::IonValue(IonDocumentPrivate& document, const StaticString& string, const ByReference&)
		: _type(Type::String)
		, _string(string, ByReference(), &document.allocator())
		, _list(document)
	{
	}

	IonValue::IonValue(IonDocumentPrivate& document, IonObject* object)
		: _type(Type::Object)
		, _string(&document.allocator())
		, _list(document)
		, _object(object)
	{
	}
}
