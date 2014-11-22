/// \file
/// \brief

#ifndef __Y_ION_VALUE_H
#define __Y_ION_VALUE_H

#include <yttrium/ion/list.h>
#include <yttrium/string.h>

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
	bool get(const StaticString** value) const noexcept
	{
		if (_type != StringType)
			return false;
		*value = &_string;
		return true;
	}

	/**
	* \overload
	*/
	bool get(int32_t* value) const noexcept { return _type == StringType && _string.to_number(value); }

	/**
	* \overload
	*/
	bool get(float* value) const noexcept { return _type == StringType && _string.to_number(value); }

	/**
	* \overload
	*/

	 bool get(const List** value) const noexcept
	{
		if (_type != ListType)
			return false;
		*value = &_list; // NOTE: Something's wrong here.
		return true;
	}

	/**
	* \overload
	*/
	bool get(const Object** value) const noexcept
	{
		if (_type != ObjectType)
			return false;
		*value = _object;
		return true;
	}

	///
	bool is_list() const noexcept { return _type == ListType; }

	///
	bool is_object() const noexcept { return _type == ObjectType; }

	///
	bool is_string() const noexcept { return _type == StringType; }

	///
	List &list() noexcept { return _list; }

	///
	const List& list() const noexcept { return _list; }

	///
	void serialize(String* result, int indentation = 0) const noexcept;

	///
	String serialize(int indentation = 0, Allocator* allocator = nullptr) const noexcept;

	///
	const StaticString& string() const noexcept { return _string; }

	///
	Type type() const noexcept { return _type; }

	///
	const Object* object() const noexcept { return _object; }

private:

	Y_PRIVATE Value(Document* document) noexcept;
	Y_PRIVATE Value(Document* document, const StaticString& string) noexcept;
	Y_PRIVATE Value(Document* document, const StaticString& string, const ByReference&) noexcept;
	Y_PRIVATE Value(Document* document, Object* object) noexcept;

private:

	Type    _type;
	String  _string;
	List    _list;
	Object* _object;
	Value*  _next;
	Value*  _previous;
};

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_VALUE_H
