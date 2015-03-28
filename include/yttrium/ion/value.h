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
	bool get(const StaticString** value) const
	{
		if (_type != StringType)
			return false;
		*value = &_string;
		return true;
	}

	/**
	* \overload
	*/
	bool get(int32_t* value) const { return _type == StringType && _string.to_number(value); }

	/**
	* \overload
	*/
	bool get(float* value) const { return _type == StringType && _string.to_number(value); }

	/**
	* \overload
	*/

	 bool get(const List** value) const
	{
		if (_type != ListType)
			return false;
		*value = &_list; // NOTE: Something's wrong here.
		return true;
	}

	/**
	* \overload
	*/
	bool get(const Object** value) const
	{
		if (_type != ObjectType)
			return false;
		*value = _object;
		return true;
	}

	///
	bool is_list() const { return _type == ListType; }

	///
	bool is_object() const { return _type == ObjectType; }

	///
	bool is_string() const { return _type == StringType; }

	///
	List &list() { return _list; }

	///
	const List& list() const { return _list; }

	///
	void serialize(String* result, int indentation = 0) const;

	///
	String serialize(int indentation = 0, Allocator* allocator = nullptr) const;

	///
	const StaticString& string() const { return _string; }

	///
	Type type() const { return _type; }

	///
	const Object* object() const { return _object; }

private:

	Y_PRIVATE Value(Document* document);
	Y_PRIVATE Value(Document* document, const StaticString& string);
	Y_PRIVATE Value(Document* document, const StaticString& string, const ByReference&);
	Y_PRIVATE Value(Document* document, Object* object);

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
