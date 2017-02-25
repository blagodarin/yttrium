/// \file
/// \brief

#ifndef _include_yttrium_ion_list_h_
#define _include_yttrium_ion_list_h_

#include <yttrium/api.h>

#include <cstddef>

namespace Yttrium
{
	class ByReference;
	class IonDocumentImpl;
	class IonListIterator;
	class IonListRange;
	class IonObject;
	class IonParser;
	class IonValue;
	class StaticString;

	///
	class Y_API IonList
	{
		friend IonDocumentImpl;
		friend IonParser;
		friend IonValue;

	public:

		///
		IonList* append_list();

		///
		IonObject* append_object();

		///
		IonValue* append(const StaticString& string);

		///
		IonListIterator begin() const;

		///
		IonListIterator end() const;

		///
		const IonValue* first() const { return _first; }

		///
		bool is_empty() const { return !_size; }

		///
		const IonValue* last() const { return _last; }

		///
		size_t size() const { return _size; }

		///
		IonListRange values() const;

		IonList(const IonList&) = delete;
		IonList& operator=(const IonList&) = delete;

	protected:
		IonList(IonDocumentImpl& document): _document(document) {}

		const IonDocumentImpl& document() const { return _document; }

	private:
		Y_PRIVATE IonValue* append(const StaticString& string, const ByReference&);
		Y_PRIVATE void append(IonValue* value);

	private:
		IonDocumentImpl& _document;
		IonValue* _first = nullptr;
		IonValue* _last = nullptr;
		size_t _size = 0;
	};

	class Y_API IonListIterator
	{
		friend IonList;
	public:
		void operator++();
		const IonValue& operator*() const { return *_value; }
		const IonValue* operator->() const { return _value; }
		bool operator==(const IonListIterator& iterator) const { return _value == iterator._value; }
		bool operator!=(const IonListIterator& iterator) const { return _value != iterator._value; }
		IonListIterator next() const;
	private:
		const IonValue* _value;
		IonListIterator(const IonValue* value): _value(value) {}
	};

	class Y_API IonListRange
	{
		friend IonList;
	public:
		IonListRange() = default;
		const IonValue& first() const { return *_first; }
		bool is_empty() const { return !_size; }
		const IonValue& last() const { return *_last; }
		void pop_first();
		void pop_last();
		size_t size() const { return _size; }
		const IonValue* operator ->() const { return _first; }
	private:
		const IonValue* _first = nullptr;
		const IonValue* _last = nullptr;
		size_t _size = 0;
		IonListRange(const IonValue* first, const IonValue* last, size_t size): _first(first), _last(last), _size(size) {}
	};
}

#endif
