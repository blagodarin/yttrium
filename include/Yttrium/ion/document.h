/// \file
/// \brief

#ifndef __Y_ION_DOCUMENT_H
#define __Y_ION_DOCUMENT_H

#include <Yttrium/allocator.h>
#include <Yttrium/ion/node.h>
#include <Yttrium/ion/object.h>
#include <Yttrium/ion/value.h>
#include <Yttrium/pool.h>

#include <list>

namespace Yttrium
{

namespace Ion
{

class Parser;

///

class Y_API Document: public Object
{
	friend class List;
	friend class Node;
	friend class Object;
	friend class Parser;
	friend class Value;

public:

	///

	Document(Allocator *allocator = DefaultAllocator) noexcept;

	///

	~Document() noexcept
	{
		clear();
	}

	///

	void clear() noexcept;

	///

	bool load(const StaticString &name) noexcept;

	///

	void save(const StaticString &name, int indentation = 0) const noexcept;

private:

	Y_PRIVATE Value *new_list_value();

	Y_PRIVATE Node *new_node(const StaticString &name);

	Y_PRIVATE Node *new_node(const StaticString &name, const String::Reference &);

	Y_PRIVATE Object *new_object();

	Y_PRIVATE Value *new_object_value(Object *object);

	Y_PRIVATE Value *new_value(const StaticString &text);

	Y_PRIVATE Value *new_value(const StaticString &name, const String::Reference &);

private:

	typedef Pool<Object> Objects;
	typedef Pool<Node>   Nodes;
	typedef Pool<Value>  Values;

private:

	mutable Allocator *_allocator;

	String  _buffer;
	Objects _objects;
	Nodes   _nodes;
	Values  _values;
};

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_DOCUMENT_H
