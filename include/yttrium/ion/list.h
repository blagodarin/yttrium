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

		void operator ++() noexcept;

		///

		inline const Value &operator *() const noexcept;

		///

		inline bool operator !=(ConstIterator iterator) const noexcept;

	private:

		const Value *_value;

		inline ConstIterator(const Value *value) noexcept;
	};

	///

	class ConstRange
	{
		friend List;

	public:

		///

		inline ConstRange() noexcept;

	public:

		///

		inline const Value &first() const noexcept;

		///

		inline bool is_empty() const noexcept;

		///

		inline const Value &last() const noexcept;

		///

		void pop_first() noexcept;

		///

		void pop_last() noexcept;

		///

		inline size_t size() const noexcept;

	public:

		///

		inline const Value *operator ->() const noexcept;

	private:

		const Value *_first;
		const Value *_last;
		size_t       _size;
		
		inline ConstRange(const Value *first, const Value *last, size_t size) noexcept;
	};

	///

	class Iterator
	{
		friend List;

	public:

		///

		void operator ++() noexcept;

		///

		inline Value &operator *() const noexcept;

		///

		inline bool operator !=(Iterator iterator) const noexcept;

	private:

		Value *_value;

		inline Iterator(Value *value) noexcept;
	};

	///

	class Range
	{
		friend List;

	public:

		///

		inline Range() noexcept;

	public:

		///

		inline Value &first() const noexcept;

		///

		inline bool is_empty() const noexcept;

		///

		inline Value &last() const noexcept;

		///

		void pop_first() noexcept;

		///

		void pop_last() noexcept;

		///

		inline size_t size() const noexcept;

	public:

		///

		inline Value *operator ->() const noexcept;

	private:

		Value  *_first;
		Value  *_last;
		size_t  _size;
		
		inline Range(Value *first, Value *last, size_t size) noexcept;
	};

public:

	///

	List *append_list() noexcept;

	///

	List *append_list(const List &list) noexcept;

	///

	Object *append_object() noexcept;

	///

	Object *append_object(const Object &object) noexcept;

	///

	Value *append(const StaticString &string) noexcept;

	///

	inline Iterator begin() noexcept;

	///

	inline ConstIterator begin() const noexcept;

	///

	void concatenate(const List &list) noexcept;

	///

	inline ConstRange const_values() const noexcept;

	///

	inline const Document *document() const noexcept;

	///

	inline Iterator end() noexcept;

	///

	inline ConstIterator end() const noexcept;

	///

	inline bool exists() const noexcept;

	///

	inline const Value *first() const noexcept;

	///

	bool first(const StaticString **value) const noexcept;

	///

	inline bool is_empty() const noexcept;

	///

	inline const Value *last() const noexcept;

	///

	inline void serialize(String *result, int indentation = 0) const noexcept;

	///

	String serialize(int indentation = 0, Allocator *allocator = nullptr) const noexcept;

	///

	String serialize(Allocator *allocator) const noexcept;

	///

	inline size_t size() const noexcept;

	///

	inline Range values() noexcept;

	///

	inline ConstRange values() const noexcept;

protected:

	inline List(Document *document) noexcept;

	Y_PRIVATE void serialize(String *result, int indentation, bool is_node) const noexcept;

private:

	Y_PRIVATE Value *append(const StaticString &string, const ByReference &) noexcept;
	Y_PRIVATE void append(Value *value) noexcept;

private:

	Document *_document;
	Value    *_first;
	Value    *_last;
	size_t    _size;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Value &List::ConstIterator::operator *() const noexcept
{
	return *_value;
}

bool List::ConstIterator::operator !=(ConstIterator iterator) const noexcept
{
	return _value != iterator._value;
}

List::ConstIterator::ConstIterator(const Value *value) noexcept
	: _value(value)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

List::ConstRange::ConstRange() noexcept
	: ConstRange(nullptr, nullptr, 0)
{
}

const Value &List::ConstRange::first() const noexcept
{
	return *_first;
}

bool List::ConstRange::is_empty() const noexcept
{
	return !_size;
}

const Value &List::ConstRange::last() const noexcept
{
	return *_last;
}

size_t List::ConstRange::size() const noexcept
{
	return _size;
}

const Value *List::ConstRange::operator ->() const noexcept
{
	return _first;
}

List::ConstRange::ConstRange(const Value *first, const Value *last, size_t size) noexcept
	: _first(first)
	, _last(last)
	, _size(size)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Value &List::Iterator::operator *() const noexcept
{
	return *_value;
}

bool List::Iterator::operator !=(Iterator iterator) const noexcept
{
	return _value != iterator._value;
}

List::Iterator::Iterator(Value *value) noexcept
	: _value(value)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

List::Range::Range() noexcept
	: Range(nullptr, nullptr, 0)
{
}

Value &List::Range::first() const noexcept
{
	return *_first;
}

bool List::Range::is_empty() const noexcept
{
	return !_size;
}

Value &List::Range::last() const noexcept
{
	return *_last;
}

size_t List::Range::size() const noexcept
{
	return _size;
}

Value *List::Range::operator ->() const noexcept
{
	return _first;
}

List::Range::Range(Value *first, Value *last, size_t size) noexcept
	: _first(first)
	, _last(last)
	, _size(size)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

List::Iterator List::begin() noexcept
{
	return Iterator(_first);
}

List::ConstIterator List::begin() const noexcept
{
	return ConstIterator(_first);
}

List::ConstRange List::const_values() const noexcept
{
	return ConstRange(_first, _last, _size);
}

const Document *List::document() const noexcept
{
	return _document;
}

List::Iterator List::end() noexcept
{
	return Iterator(nullptr);
}

List::ConstIterator List::end() const noexcept
{
	return ConstIterator(nullptr);
}

bool List::exists() const noexcept
{
	return _document;
}

const Value *List::first() const noexcept
{
	return _first;
}

bool List::is_empty() const noexcept
{
	return !_size;
}

const Value *List::last() const noexcept
{
	return _last;
}

size_t List::size() const noexcept
{
	return _size;
}

void List::serialize(String *result, int indentation) const noexcept
{
	serialize(result, indentation, false);
}

List::Range List::values() noexcept
{
	return Range(_first, _last, _size);
}

List::ConstRange List::values() const noexcept
{
	return ConstRange(_first, _last, _size);
}

List::List(Document *document) noexcept
	: _document(document)
	, _first(nullptr)
	, _last(nullptr)
	, _size(0)
{
}

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_LIST_H
