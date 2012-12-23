#include <Yttrium/ion/list.h>

#include <Yttrium/ion/document.h>
#include <Yttrium/ion/value.h>

namespace Yttrium
{

namespace Ion
{

void List::Range::pop_first()
{
	_first = _first->_next;
	--_size;
}

void List::ConstRange::pop_first()
{
	_first = _first->_next;
	--_size;
}

List *List::append_list()
{
	Value *new_value = _document->new_list_value();
	List *new_list = &new_value->list();
	append(new_value);
	return new_list;
}

List *List::append_list(const List &list)
{
	Value *new_value = _document->new_list_value();
	List *new_list = &new_value->list();
	new_list->concatenate(list);
	append(new_value);
	return new_list;
}

Object *List::append_object()
{
	Object *new_object = _document->new_object();
	append(_document->new_object_value(new_object));
	return new_object;
}

Object *List::append_object(const Object &object)
{
	Object *new_object = _document->new_object();
	new_object->concatenate(object);
	append(_document->new_object_value(new_object));
	return new_object;
}

Value *List::append(const StaticString &string)
{
	Value *value = _document->new_value(string);
	append(value);
	return value;
}

void List::concatenate(const List &list)
{
	for (ConstRange r = list.values(); !r.is_empty(); r.pop_first())
	{
		const Value &value = r.first();

		switch (value.type())
		{
		case Value::ListType:

			append_list(value.list());
			break;

		case Value::ObjectType:

			append_object(*value.object());
			break;

		default:

			append(_document->new_value(value.string()));
			break;
		}
	}
}

bool List::first(const StaticString **value) const
{
	if (_first && _first->is_string())
	{
		*value = &_first->string();
		return true;
	}
	return false;
}

String List::serialize(int indentation, Allocator *allocator) const
{
	String result(allocator ? allocator : _document->allocator());

	serialize(&result, indentation, false);
	return result;
}

String List::serialize(Allocator *allocator) const
{
	String result(allocator ? allocator : _document->allocator());

	serialize(&result, 0, false);
	return result;
}

void List::serialize(String *result, int indentation, bool node) const
{
	if (!node)
	{
		result->append('[');
	}

	const Value *value = _first;

	if (value)
	{
		if (node && indentation >= 0 && !value->is_object())
		{
			result->append(' ');
		}

		for (; ; )
		{
			value->serialize(result, indentation);
			value = value->_next;
			if (!value)
			{
				break;
			}
			if (indentation >= 0 && !value->is_object())
			{
				result->append(' ');
			}
		}
	}

	if (!node)
	{
		result->append(']');
	}
}

Value *List::append(const StaticString &string, const ByReference &)
{
	Value *value = _document->new_value(string, ByReference());
	append(value);
	return value;
}

void List::append(Value *value)
{
	if (!_first)
	{
		_first = value;
	}
	else
	{
		_last->_next = value;
	}
	_last = value;
	++_size;
}

} // namespace Ion

} // namespace Yttrium
