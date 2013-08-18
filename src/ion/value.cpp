#include <yttrium/ion/value.h>

#include <yttrium/ion/document.h>

namespace Yttrium
{

namespace Ion
{

void Value::serialize(String *result, int indentation) const
{
	switch (_type)
	{
	case ListType:

		_list.serialize(result, indentation + (indentation >= 0));
		break;

	case ObjectType:

		_object->serialize(result, indentation);
		break;

	default:

		result->append('"').append(_string.escaped("\\\"", '\\')).append('"');
		break;
	}
}

String Value::serialize(int indentation, Allocator *allocator) const
{
	String result(allocator ? allocator : _string.allocator());

	serialize(&result, indentation);
	return result;
}

Value::Value(Document *document)
	: _type(ListType)
	, _string(document->allocator())
	, _list(document)
	, _object(nullptr)
	, _next(nullptr)
{
}

Value::Value(Document *document, const StaticString &string)
	: _type(StringType)
	, _string(string, document->allocator())
	, _list(document)
	, _object(nullptr)
	, _next(nullptr)
{
}

Value::Value(Document *document, const StaticString &string, const ByReference &)
	: _type(StringType)
	, _string(string, ByReference(), document->allocator())
	, _list(document)
	, _object(nullptr)
	, _next(nullptr)
{
}

Value::Value(Document *document, Object *object)
	: _type(ObjectType)
	, _string(document->allocator())
	, _list(document)
	, _object(object)
	, _next(nullptr)
{
}

} // namespace Ion

} // namespace Yttrium
