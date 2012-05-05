/// \file
/// \brief

#ifndef __Y_ION_DOCUMENT_HPP
#define __Y_ION_DOCUMENT_HPP

#include <list> // std::list

#include <Yttrium/allocator.hpp>
#include <Yttrium/ion/node.hpp>
#include <Yttrium/ion/object.hpp>
#include <Yttrium/ion/value.hpp>
#include <Yttrium/pool.hpp>

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

	Document(Allocator *allocator = HeapAllocator::instance()) noexcept;

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

	Value *new_list_value();

	Node *new_node(const StaticString &name);

	Node *new_node(const StaticString &name, const String::Reference &);

	Object *new_object();

	Value *new_object_value(Object *object);

	Value *new_value(const StaticString &text);

	Value *new_value(const StaticString &name, const String::Reference &);

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

#endif // __FALX_ION_DOCUMENT_HPP
