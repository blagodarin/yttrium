/// \file
/// \brief

#ifndef __Y_ION_VALUE_HPP
#define __Y_ION_VALUE_HPP

#include <Yttrium/ion/indentation.hpp>
#include <Yttrium/ion/list.hpp>
#include <Yttrium/static_string.hpp>
#include <Yttrium/string.hpp>

namespace Yttrium
{

namespace Ion
{

class Document;

///

class Y_API Value
{
	friend class Document;
	friend class List;
	friend class List::ConstRange;
	friend class List::Range;

public:

	///

	enum Type
	{
		StringType, ///<
		ListType,   ///<
		ObjectType  ///<
	};

public:

	//Value(const Value &) = delete; // TODO: Uncomment on Pool porting.

	Value &operator =(const Value &) = delete;

	///

	bool is_list() const noexcept
	{
		return (_type == ListType);
	}

	///

	bool is_object() const noexcept
	{
		return (_type == ObjectType);
	}

	///

	bool is_string() const noexcept
	{
		return (_type == StringType);
	}

	///

	List &list() noexcept
	{
		return _list;
	}

	///

	const List &list() const noexcept
	{
		return _list;
	}

	///

	const StaticString &string() const noexcept
	{
		return _string;
	}

	///

	Type type() const noexcept
	{
		return _type;
	}

	///

	const Object *object() const noexcept
	{
		return _object;
	}

	///

	void to_string(String *result, int indentation = 0) const noexcept;

	///

	String to_string(int indentation = 0) const noexcept
	{
		String result(_string.allocator()); // NOTE: Wrong allocator?

		to_string(&result, indentation);
		return result;
	}

private:

	Value(Document *document);

	Value(Document *document, const StaticString &string);

	Value(Document *document, const StaticString &string, const String::Reference &);

	Value(Document *document, Object *object);

private:

	Type    _type;
	String  _string;
	List    _list;
	Object *_object;
	Value  *_next;
};

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_VALUE_HPP
