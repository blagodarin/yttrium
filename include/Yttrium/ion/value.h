/// \file
/// \brief

#ifndef __Y_ION_VALUE_H
#define __Y_ION_VALUE_H

#include <Yttrium/ion/list.h>
#include <Yttrium/string.h>

namespace Yttrium
{

namespace Ion
{

class Document;

///

class Y_API Value
{
	friend Document;
	friend List;

	Y_NONCOPYABLE(Value);

public:

	///

	enum Type
	{
		StringType, ///<
		ListType,   ///<
		ObjectType, ///<
	};

public:

	///

	inline bool get(const StaticString **value) const noexcept;

	/**
	* \overload
	*/

	inline bool get(Integer *value) const noexcept;

	/**
	* \overload
	*/

	inline bool get(float *value) const noexcept;

	/**
	* \overload
	*/

	inline bool get(const List **value) const noexcept;

	/**
	* \overload
	*/

	inline bool get(const Object **value) const noexcept;

	///

	inline bool is_list() const noexcept;

	///

	inline bool is_object() const noexcept;

	///

	inline bool is_string() const noexcept;

	///

	inline List &list() noexcept;

	///

	inline const List &list() const noexcept;

	///

	void serialize(String *result, int indentation = 0) const noexcept;

	///

	String serialize(int indentation = 0, Allocator *allocator = nullptr) const noexcept;

	///

	inline const StaticString &string() const noexcept;

	///

	inline Type type() const noexcept;

	///

	inline const Object *object() const noexcept;

private:

	Y_PRIVATE Value(Document *document) noexcept;
	Y_PRIVATE Value(Document *document, const StaticString &string) noexcept;
	Y_PRIVATE Value(Document *document, const StaticString &string, const ByReference &) noexcept;
	Y_PRIVATE Value(Document *document, Object *object) noexcept;

private:

	Type    _type;
	String  _string;
	List    _list;
	Object *_object;
	Value  *_next;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Value::get(const StaticString **value) const noexcept
{
	if (_type == StringType)
	{
		*value = &_string;
		return true;
	}
	return false;
}

bool Value::get(Integer *value) const noexcept
{
	return _type == StringType && _string.to_number(value);
}

bool Value::get(float *value) const noexcept
{
	return _type == StringType && _string.to_number(value);
}

bool Value::get(const List **value) const noexcept
{
	if (_type == ListType)
	{
		*value = &_list; // NOTE: Something's wrong here.
		return true;
	}
	return false;
}

bool Value::get(const Object **value) const noexcept
{
	if (_type == ObjectType)
	{
		*value = _object;
		return true;
	}
	return false;
}

bool Value::is_list() const noexcept
{
	return _type == ListType;
}

bool Value::is_object() const noexcept
{
	return _type == ObjectType;
}

bool Value::is_string() const noexcept
{
	return _type == StringType;
}

List &Value::list() noexcept
{
	return _list;
}

const List &Value::list() const noexcept
{
	return _list;
}

const StaticString &Value::string() const noexcept
{
	return _string;
}

Value::Type Value::type() const noexcept
{
	return _type;
}

const Object *Value::object() const noexcept
{
	return _object;
}

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_VALUE_H
