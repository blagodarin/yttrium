/// \file
/// \brief

#ifndef __Y_ION_DOCUMENT_H
#define __Y_ION_DOCUMENT_H

#include <Yttrium/ion/node.h>
#include <Yttrium/ion/object.h>
#include <Yttrium/ion/value.h>
#include <Yttrium/pool.h>

// NOTE: We don't need Pool in the interface but are still forced to include it
// along with Node and Value.

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

	Document(const Document &document, Allocator *allocator = nullptr) noexcept;

	///

	inline ~Document() noexcept;

public:

	///

	void clear() noexcept;

	///

	bool load(const StaticString &name) noexcept;

	///

	void save(const StaticString &name, int indentation = 0) const noexcept;

public:

	///

	inline Document &operator =(const Document &document) noexcept;

private:

	Y_PRIVATE Value *new_list_value() noexcept;
	Y_PRIVATE Node *new_node(const StaticString &name) noexcept;
	Y_PRIVATE Node *new_node(const StaticString &name, const ByReference &) noexcept;
	Y_PRIVATE Object *new_object() noexcept;
	Y_PRIVATE Value *new_object_value(Object *object) noexcept;
	Y_PRIVATE Value *new_value(const StaticString &text) noexcept;
	Y_PRIVATE Value *new_value(const StaticString &name, const ByReference &) noexcept;

private:

	typedef Pool<Object> Objects;
	typedef Pool<Node>   Nodes;
	typedef Pool<Value>  Values;

private:

	Allocator *_allocator;
	String     _buffer;
	Objects    _objects;
	Nodes      _nodes;
	Values     _values;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Document::~Document() noexcept
{
	clear();
}

Document &Document::operator =(const Document &document) noexcept
{
	clear();
	concatenate(document);
	return *this;
}

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_DOCUMENT_H
