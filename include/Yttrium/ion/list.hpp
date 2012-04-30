/// \file
/// \brief

#ifndef __Y_ION_LIST_HPP
#define __Y_ION_LIST_HPP

#include <Yttrium/noncopyable.hpp>
#include <Yttrium/static_string.hpp>
#include <Yttrium/string.hpp>

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

	Object *append_object() noexcept;

	///

	Value *append(const StaticString &string) noexcept;

	///

	bool is_empty() const noexcept
	{
		return !_size;
	}

	///

	size_t size() const noexcept
	{
		return _size;
	}

	///

	void to_string(String *result, int indentation = 0) const noexcept
	{
		to_string(result, indentation, false);
	}

	///

	String to_string(int indentation = 0) const noexcept; // NOTE: Wrong allocator?

	///

	Range values() noexcept
	{
		return Range(_first, _last, _size);
	}

	///

	ConstRange values() const noexcept
	{
		return ConstRange(_first, _last, _size);
	}

protected:

	List(Document *document)
		: _document(*document)
		, _first(nullptr)
		, _last(nullptr)
		, _size(0)
	{
	}

	void to_string(String *result, int indentation, bool node) const;

private:

	Value *append(const StaticString &string, const String::Reference &);

	void append(Value *value);

private:

	Document &_document;
	Value    *_first;
	Value    *_last;
	size_t    _size;
};

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_LIST_HPP
