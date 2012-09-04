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

// TODO: Think about merging append_list() and append_list(const List *).

List *List::append_list()
{
	Value *value = _document.new_list_value();
	append(value);
	return &value->list();
}

List *List::append_list(const List *list)
{
	List *new_list = append_list();

	// NOTE: This looks way to similar to Object::append(const Node *).

	for (ConstRange r = list->values(); !r.is_empty(); r.pop_first())
	{
		const Value &value = r.first();

		switch (value.type())
		{
		case Value::ListType:

			new_list->append_list(&value.list());
			break;

		case Value::ObjectType:

			new_list->append_object(value.object());
			break;

		default:

			new_list->append(value.string());
			break;
		}
	}

	return new_list;
}

// TODO: Think about merging append_object() and append_object(const Object *).

Object* List::append_object()
{
	Object *object = _document.new_object();
	append(_document.new_object_value(object));
	return object;
}

Object *List::append_object(const Object *object)
{
	Object *new_object = append_object();

	for (Object::ConstRange r = object->nodes(); !r.is_empty(); r.pop_first())
	{
		new_object->append(&r.first());
	}

	return new_object;
}

Value *List::append(const StaticString &string)
{
	Value *value = _document.new_value(string);
	append(value);
	return value;
}

String List::to_string(int indentation, Allocator *allocator) const
{
	String result(allocator ? allocator : _document._allocator);

	to_string(&result, indentation, false);
	return result;
}

void List::to_string(String *result, int indentation, bool node) const
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
			value->to_string(result, indentation);
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

Value *List::append(const StaticString &string, const String::Reference &)
{
	Value *value = _document.new_value(string, String::Ref);
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
