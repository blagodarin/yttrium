#include <yttrium/ion/value.h>

#include "document.h"

namespace Yttrium
{
	IonValue::IonValue(IonDocumentImpl& document)
		: _type{Type::List}
		, _list{document}
	{
	}

	IonValue::IonValue(IonDocumentImpl& document, std::string_view string)
		: _type{Type::String}
		, _string{string}
		, _list{document}
	{
	}

	IonValue::IonValue(IonDocumentImpl& document, std::string_view string, const ByReference&)
		: _type{Type::String}
		, _string{string, ByReference{}}
		, _list{document}
	{
	}

	IonValue::IonValue(IonDocumentImpl& document, IonObject* object)
		: _type{Type::Object}
		, _list{document}
		, _object{object}
	{
	}
}
