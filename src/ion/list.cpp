#include <yttrium/ion/list.h>

#include <yttrium/ion/value.h>
#include "document.h"

namespace Yttrium
{
	void IonListIterator::operator ++()
	{
		_value = _value->_next;
	}

	void IonListRange::pop_first()
	{
		_first = _first->_next;
		--_size;
	}

	void IonListRange::pop_last()
	{
		_last = _last->_previous;
		--_size;
	}

	IonList* IonList::append_list()
	{
		IonValue* new_value = _document.new_list_value();
		IonList* new_list = &new_value->list();
		append(new_value);
		return new_list;
	}

	IonObject* IonList::append_object()
	{
		IonObject* new_object = _document.new_object();
		append(_document.new_object_value(new_object));
		return new_object;
	}

	IonValue* IonList::append(const StaticString& string)
	{
		IonValue* value = _document.new_value(string);
		append(value);
		return value;
	}

	IonListIterator IonList::begin() const
	{
		return IonListIterator(_first);
	}

	IonListIterator IonList::end() const
	{
		return IonListIterator(nullptr);
	}

	IonListRange IonList::values() const
	{
		return IonListRange(_first, _last, _size);
	}

	IonValue* IonList::append(const StaticString& string, const ByReference&)
	{
		IonValue* value = _document.new_value(string, ByReference());
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
