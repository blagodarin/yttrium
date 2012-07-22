/// \file
/// \brief

#ifndef __Y_ION_VALUE_H
#define __Y_ION_VALUE_H

#include <Yttrium/ion/list.h>
#include <Yttrium/noncopyable.h>
#include <Yttrium/string.h>

namespace Yttrium
{

namespace Ion
{

class Document;

///

class Y_API Value: public Noncopyable
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

	Y_PRIVATE Value(Document *document);

	Y_PRIVATE Value(Document *document, const StaticString &string);

	Y_PRIVATE Value(Document *document, const StaticString &string, const String::Reference &);

	Y_PRIVATE Value(Document *document, Object *object);

private:

	Type    _type;
	String  _string;
	List    _list;
	Object *_object;
	Value  *_next;
};

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_VALUE_H
