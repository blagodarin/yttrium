/// \file
/// \brief

#ifndef __Y_ION_LIST_H
#define __Y_ION_LIST_H

#include <yttrium/static_string.h>

namespace Yttrium
{

class String;

namespace Ion
{

class Document;
class Object;
class Parser;
class Value;

///

class Y_API List
{
	friend Parser;
	friend Value;

	Y_NONCOPYABLE(List);

public:

	///

	class ConstIterator
	{
		friend List;

	public:

		///

		void operator ++();

		///

		inline const Value &operator *() const;

		///

		inline bool operator !=(ConstIterator iterator) const;

	private:

		const Value *_value;

		inline ConstIterator(const Value *value);
	};

	///

	class ConstRange
	{
		friend List;

	public:

		///

		inline ConstRange();

	public:

		///

		inline const Value &first() const;

		///

		inline bool is_empty() const;

		///

		inline const Value &last() const;

		///

		void pop_first();

		///

		void pop_last();

		///

		inline size_t size() const;

	public:

		///

		inline const Value *operator ->() const;

	private:

		const Value *_first;
		const Value *_last;
		size_t       _size;
		
		inline ConstRange(const Value *first, const Value *last, size_t size);
	};

	///

	class Iterator
	{
		friend List;

	public:

		///

		void operator ++();

		///

		inline Value &operator *() const;

		///

		inline bool operator !=(Iterator iterator) const;

	private:

		Value *_value;

		inline Iterator(Value *value);
	};

	///

	class Range
	{
		friend List;

	public:

		///

		inline Range();

	public:

		///

		inline Value &first() const;

		///

		inline bool is_empty() const;

		///

		inline Value &last() const;

		///

		void pop_first();

		///

		void pop_last();

		///

		inline size_t size() const;

	public:

		///

		inline Value *operator ->() const;

	private:

		Value  *_first;
		Value  *_last;
		size_t  _size;
		
		inline Range(Value *first, Value *last, size_t size);
	};

public:

	///

	List *append_list();

	///

	List *append_list(const List &list);

	///

	Object *append_object();

	///

	Object *append_object(const Object &object);

	///

	Value *append(const StaticString &string);

	///

	inline Iterator begin();

	///

	inline ConstIterator begin() const;

	///

	void concatenate(const List &list);

	///

	inline ConstRange const_values() const;

	///

	inline const Document *document() const;

	///

	inline Iterator end();

	///

	inline ConstIterator end() const;

	///

	inline bool exists() const;

	///

	inline const Value *first() const;

	///

	bool first(const StaticString **value) const;

	///

	inline bool is_empty() const;

	///

	inline const Value *last() const;

	///

	inline void serialize(String *result, int indentation = 0) const;

	///

	String serialize(int indentation = 0, Allocator *allocator = nullptr) const;

	///

	String serialize(Allocator *allocator) const;

	///

	inline size_t size() const;

	///

	inline Range values();

	///

	inline ConstRange values() const;

protected:

	inline List(Document *document);

	Y_PRIVATE void serialize(String *result, int indentation, bool is_node) const;

private:

	Y_PRIVATE Value *append(const StaticString &string, const ByReference &);
	Y_PRIVATE void append(Value *value);

private:

	Document *_document;
	Value    *_first;
	Value    *_last;
	size_t    _size;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Value &List::ConstIterator::operator *() const
{
	return *_value;
}

bool List::ConstIterator::operator !=(ConstIterator iterator) const
{
	return _value != iterator._value;
}

List::ConstIterator::ConstIterator(const Value *value)
	: _value(value)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

List::ConstRange::ConstRange()
	: ConstRange(nullptr, nullptr, 0)
{
}

const Value &List::ConstRange::first() const
{
	return *_first;
}

bool List::ConstRange::is_empty() const
{
	return !_size;
}

const Value &List::ConstRange::last() const
{
	return *_last;
}

size_t List::ConstRange::size() const
{
	return _size;
}

const Value *List::ConstRange::operator ->() const
{
	return _first;
}

List::ConstRange::ConstRange(const Value *first, const Value *last, size_t size)
	: _first(first)
	, _last(last)
	, _size(size)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Value &List::Iterator::operator *() const
{
	return *_value;
}

bool List::Iterator::operator !=(Iterator iterator) const
{
	return _value != iterator._value;
}

List::Iterator::Iterator(Value *value)
	: _value(value)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

List::Range::Range()
	: Range(nullptr, nullptr, 0)
{
}

Value &List::Range::first() const
{
	return *_first;
}

bool List::Range::is_empty() const
{
	return !_size;
}

Value &List::Range::last() const
{
	return *_last;
}

size_t List::Range::size() const
{
	return _size;
}

Value *List::Range::operator ->() const
{
	return _first;
}

List::Range::Range(Value *first, Value *last, size_t size)
	: _first(first)
	, _last(last)
	, _size(size)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

List::Iterator List::begin()
{
	return Iterator(_first);
}

List::ConstIterator List::begin() const
{
	return ConstIterator(_first);
}

List::ConstRange List::const_values() const
{
	return ConstRange(_first, _last, _size);
}

const Document *List::document() const
{
	return _document;
}

List::Iterator List::end()
{
	return Iterator(nullptr);
}

List::ConstIterator List::end() const
{
	return ConstIterator(nullptr);
}

bool List::exists() const
{
	return _document;
}

const Value *List::first() const
{
	return _first;
}

bool List::is_empty() const
{
	return !_size;
}

const Value *List::last() const
{
	return _last;
}

size_t List::size() const
{
	return _size;
}

void List::serialize(String *result, int indentation) const
{
	serialize(result, indentation, false);
}

List::Range List::values()
{
	return Range(_first, _last, _size);
}

List::ConstRange List::values() const
{
	return ConstRange(_first, _last, _size);
}

List::List(Document *document)
	: _document(document)
	, _first(nullptr)
	, _last(nullptr)
	, _size(0)
{
}

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_LIST_H
