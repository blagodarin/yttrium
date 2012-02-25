#include <Yttrium/ion/list.hpp>

#include <Yttrium/ion/document.hpp>
#include <Yttrium/ion/value.hpp>

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
	Value *value = _document.new_list_value();
	append(value);
	return &value->list();
}

Object* List::append_object()
{
	Object *object = _document.new_object();
	append(_document.new_object_value(object));
	return object;
}

Value *List::append(const StaticString &string)
{
	Value *value = _document.new_value(string);
	append(value);
	return value;
}

String List::to_string(int indentation) const
{
	String result(_document._allocator);

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
		for (; ; )
		{
			value->to_string(result, indentation);
			value = value->_next;
			if (!value)
			{
				break;
			}
			if (indentation >= 0)
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
