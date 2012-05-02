#include <Yttrium/ion/value.hpp>

#include <Yttrium/ion/document.hpp>

namespace Yttrium
{

namespace Ion
{

void Value::to_string(String *result, int indentation) const
{
	switch (_type)
	{
	case ListType:

		_list.to_string(result, indentation + (indentation >= 0));
		break;

	case ObjectType:

		_object->to_string(result, indentation);
		break;

	default:

		result->append('"').append(_string.escaped("\\\"", '\\')).append('"');
		break;
	}
}

Value::Value(Document *document)
	: _type(ListType)
	, _string(document->_allocator)
	, _list(document)
	, _object(nullptr)
	, _next(nullptr)
{
}

Value::Value(Document *document, const StaticString &string)
	: _type(StringType)
	, _string(string, document->_allocator)
	, _list(document)
	, _object(nullptr)
	, _next(nullptr)
{
}

Value::Value(Document *document, const StaticString &string, const String::Reference &)
	: _type(StringType)
	, _string(string, String::Ref, document->_allocator)
	, _list(document)
	, _object(nullptr)
	, _next(nullptr)
{
}

Value::Value(Document *document, Object *object)
	: _type(ObjectType)
	, _string(document->_allocator)
	, _list(document)
	, _object(object)
	, _next(nullptr)
{
}

} // namespace Ion

} // namespace Yttrium