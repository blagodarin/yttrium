#include <yttrium/ion/list.h>

#include <yttrium/ion/value.h>
#include "document.h"

namespace Yttrium
{
	void IonListIterator::operator++()
	{
		_value = _value->_next;
	}

	IonListIterator IonListIterator::next() const
	{
		return IonListIterator{_value->_next};
	}

	void IonListRange::pop_first()
	{
		_first = _first->_next;
		--_size;
	}

	IonList* IonList::append_list()
	{
		const auto new_value = _document.new_list_value();
		const auto new_list = &new_value->list();
		append(new_value);
		return new_list;
	}

	IonObject* IonList::append_object()
	{
		const auto new_object = _document.new_object();
		append(_document.new_object_value(new_object));
		return new_object;
	}

	IonValue* IonList::append(std::string_view string)
	{
		const auto value = _document.new_value(string);
		append(value);
		return value;
	}

	IonListIterator IonList::begin() const
	{
		return IonListIterator{_first};
	}

	IonListIterator IonList::end() const
	{
		return IonListIterator{nullptr};
	}

	IonListRange IonList::values() const
	{
		return IonListRange{_first, _last, _size};
	}

	IonValue* IonList::append(std::string_view string, const ByReference&)
	{
		const auto value = _document.new_value(string, ByReference{});
		append(value);
		return value;
	}

	void IonList::append(IonValue* value)
	{
		if (_first)
		{
			_last->_next = value;
			value->_previous = _last;
		}
		else
			_first = value;
		_last = value;
		++_size;
	}
}
