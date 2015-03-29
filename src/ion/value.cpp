#include <yttrium/ion/value.h>

#include <yttrium/ion/document.h>

namespace Yttrium
{
	void IonValue::serialize(String* result, int indentation) const
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

	String IonValue::serialize(int indentation, Allocator* allocator) const
	{
		String result(allocator ? allocator : _string.allocator());
		serialize(&result, indentation);
		return std::move(result);
	}

	IonValue::IonValue(IonDocument* document)
		: _type(ListType)
		, _string(document->allocator())
		, _list(document)
		, _object(nullptr)
		, _next(nullptr)
		, _previous(nullptr)
	{
	}

	IonValue::IonValue(IonDocument* document, const StaticString& string)
		: _type(StringType)
		, _string(string, document->allocator())
		, _list(document)
		, _object(nullptr)
		, _next(nullptr)
		, _previous(nullptr)
	{
	}

	IonValue::IonValue(IonDocument* document, const StaticString& string, const ByReference&)
		: _type(StringType)
		, _string(string, ByReference(), document->allocator())
		, _list(document)
		, _object(nullptr)
		, _next(nullptr)
		, _previous(nullptr)
	{
	}

	IonValue::IonValue(IonDocument* document, IonObject* object)
		: _type(ObjectType)
		, _string(document->allocator())
		, _list(document)
		, _object(object)
		, _next(nullptr)
		, _previous(nullptr)
	{
	}
}
