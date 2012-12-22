/// \file
/// \brief

#ifndef __Y_ION_LIST_H
#define __Y_ION_LIST_H

#include <Yttrium/noncopyable.h>
#include <Yttrium/static_string.h>

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

class Y_API List: public Noncopyable
{
	friend class Parser;
	friend class Value;

public:

	///

	class Range
	{
		friend class List;

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

		inline size_t size() const noexcept;

	private:

		inline Range(Value *first, Value *last, size_t size) noexcept;

	private:

		Value  *_first;
		Value  *_last;
		size_t  _size;
	};

	///

	class ConstRange
	{
		friend class List;

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

		inline size_t size() const noexcept;

	private:

		inline ConstRange(const Value *first, const Value *last, size_t size) noexcept;

	private:

		const Value *_first;
		const Value *_last;
		size_t       _size;
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

	void concatenate(const List &list) noexcept;

	///

	inline ConstRange const_values() const noexcept;

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

	inline size_t size() const noexcept;

	///

	inline void to_string(String *result, int indentation = 0) const noexcept;

	///

	String to_string(int indentation = 0, Allocator *allocator = nullptr) const noexcept;

	///

	String to_string(Allocator *allocator) const noexcept;

	///

	inline Range values() noexcept;

	///

	inline ConstRange values() const noexcept;

protected:

	inline List(Document *document) noexcept;

	Y_PRIVATE void to_string(String *result, int indentation, bool node) const noexcept;

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

List::Range::Range() noexcept
	//: Range(nullptr, nullptr, 0) // TODO: Uncomment.
	: _first(nullptr)
	, _last(nullptr)
	, _size(0)
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

List::Range::Range(Value *first, Value *last, size_t size) noexcept
	: _first(first)
	, _last(last)
	, _size(size)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

List::ConstRange::ConstRange() noexcept
	//: ConstRange(nullptr, nullptr, 0) // TODO: Uncomment.
	: _first(nullptr)
	, _last(nullptr)
	, _size(0)
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

List::ConstRange::ConstRange(const Value *first, const Value *last, size_t size) noexcept
	: _first(first)
	, _last(last)
	, _size(size)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

List::ConstRange List::const_values() const noexcept
{
	return ConstRange(_first, _last, _size);
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

void List::to_string(String *result, int indentation) const noexcept
{
	to_string(result, indentation, false);
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
