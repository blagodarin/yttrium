#include <yttrium/ion/list.h>

#include <yttrium/ion/value.h>
#include "document.h"

namespace Yttrium
{
	void IonList::ConstIterator::operator ++()
	{
		_value = _value->_next;
	}

	void IonList::ConstRange::pop_first()
	{
		_first = _first->_next;
		--_size;
	}

	void IonList::ConstRange::pop_last()
	{
		_last = _last->_previous;
		--_size;
	}

	IonList* IonList::append_list()
	{
		IonValue* new_value = _document->_private->new_list_value();
		IonList* new_list = &new_value->list();
		append(new_value);
		return new_list;
	}

	IonList* IonList::append_list(const IonList& list)
	{
		IonValue* new_value = _document->_private->new_list_value();
		IonList* new_list = &new_value->list();
		new_list->concatenate(list);
		append(new_value);
		return new_list;
	}

	IonObject* IonList::append_object()
	{
		IonObject* new_object = _document->_private->new_object();
		append(_document->_private->new_object_value(new_object));
		return new_object;
	}

	IonObject *IonList::append_object(const IonObject& object)
	{
		IonObject* new_object = _document->_private->new_object();
		new_object->concatenate(object);
		append(_document->_private->new_object_value(new_object));
		return new_object;
	}

	IonValue* IonList::append(const StaticString& string)
	{
		IonValue* value = _document->_private->new_value(string);
		append(value);
		return value;
	}

	void IonList::concatenate(const IonList& list)
	{
		for (const IonValue& value: list)
		{
			switch (value.type())
			{
			case IonValue::ListType:
				append_list(value.list());
				break;

			case IonValue::ObjectType:
				append_object(*value.object());
				break;

			default:
				append(_document->_private->new_value(value.string()));
				break;
			}
		}
	}

	bool IonList::first(const StaticString** value) const
	{
		if (_first && _first->is_string())
		{
			*value = &_first->string();
			return true;
		}
		return false;
	}

	String IonList::serialize(int indentation, Allocator* allocator) const
	{
		String result(allocator ? allocator : _document->allocator());
		serialize(&result, indentation, false);
		return std::move(result);
	}

	void IonList::serialize(String* result, int indentation, bool node) const
	{
		if (!node)
			result->append('[');

		const IonValue* value = _first;

		if (value)
		{
			if (node && indentation >= 0 && !value->is_object())
				result->append(' ');

			for (; ; )
			{
				value->serialize(result, indentation);
				value = value->_next;
				if (!value)
					break;
				if (indentation >= 0 && !value->is_object())
					result->append(' ');
			}
		}

		if (!node)
			result->append(']');
	}

	IonValue* IonList::append(const StaticString& string, const ByReference&)
	{
		IonValue* value = _document->_private->new_value(string, ByReference());
		append(value);
		return value;
	}

	void IonList::append(IonValue* value)
	{
		if (!_first)
		{
			_first = value;
		}
		else
		{
			_last->_next = value;
			value->_previous = _last;
		}
		_last = value;
		++_size;
	}
}
