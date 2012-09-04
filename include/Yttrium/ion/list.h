/// \file
/// \brief

#ifndef __Y_ION_LIST_H
#define __Y_ION_LIST_H

#include <Yttrium/noncopyable.h>
#include <Yttrium/string.h>

namespace Yttrium
{

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

		Value &first() const noexcept
		{
			return *_first;
		}

		///

		bool is_empty() const noexcept
		{
			return !_size;
		}

		///

		Value &last() const noexcept
		{
			return *_last;
		}

		///

		void pop_first() noexcept;

		///

		size_t size() const noexcept
		{
			return _size;
		}

	private:

		Range(Value *first, Value *last, size_t size) noexcept
			: _first(first)
			, _last(last)
			, _size(size)
		{
		}

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

		const Value &first() const noexcept
		{
			return *_first;
		}

		///

		bool is_empty() const noexcept
		{
			return !_size;
		}

		///

		const Value &last() const noexcept
		{
			return *_last;
		}

		///

		void pop_first() noexcept;

		///

		size_t size() const noexcept
		{
			return _size;
		}

	private:

		ConstRange(const Value *first, const Value *last, size_t size) noexcept
			: _first(first)
			, _last(last)
			, _size(size)
		{
		}

	private:

		const Value *_first;
		const Value *_last;
		size_t       _size;
	};

public:

	///

	List *append_list() noexcept;

	///

	List *append_list(const List *list) noexcept;

	///

	Object *append_object() noexcept;

	///

	Object *append_object(const Object *object) noexcept;

	///

	Value *append(const StaticString &string) noexcept;

	///

	inline ConstRange const_values() const noexcept;

	///

	inline bool is_empty() const noexcept;

	///

	inline size_t size() const noexcept;

	///

	inline void to_string(String *result, int indentation = 0) const noexcept;

	///

	String to_string(int indentation = 0, Allocator *allocator = nullptr) const noexcept;

	///

	inline String to_string(Allocator *allocator) const noexcept;

	///

	inline Range values() noexcept;

	///

	inline ConstRange values() const noexcept;

protected:

	inline List(Document *document) noexcept;

	Y_PRIVATE void to_string(String *result, int indentation, bool node) const;

private:

	Y_PRIVATE Value *append(const StaticString &string, const String::Reference &);

	Y_PRIVATE void append(Value *value);

private:

	Document &_document;
	Value    *_first;
	Value    *_last;
	size_t    _size;
};

////////////////////////////////////////////////////////////////////////////////

List::ConstRange List::const_values() const noexcept
{
	return ConstRange(_first, _last, _size);
}

bool List::is_empty() const noexcept
{
	return !_size;
}

size_t List::size() const noexcept
{
	return _size;
}

void List::to_string(String *result, int indentation) const noexcept
{
	to_string(result, indentation, false);
}

String List::to_string(Allocator *allocator) const noexcept
{
	return to_string(0, allocator);
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
	: _document(*document)
	, _first(nullptr)
	, _last(nullptr)
	, _size(0)
{
}

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_LIST_H
