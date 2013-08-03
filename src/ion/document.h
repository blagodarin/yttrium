#ifndef __ION_DOCUMENT_H
#define __ION_DOCUMENT_H

#include <Yttrium/ion/document.h>
#include <Yttrium/ion/node.h>
#include <Yttrium/ion/value.h>
#include <Yttrium/pool.h>

namespace Yttrium
{

namespace Ion
{

class Y_PRIVATE Document::Private
{
	friend Document;

public:

	Private(Document *document, Allocator *allocator)
		: _document(document)
		, _allocator(allocator)
		, _buffer(allocator)
		, _objects(32, allocator)
		, _nodes(32, allocator)
		, _values(32, allocator)
	{
		// TODO: Get rid of magic numbers above.
	}

	~Private()
	{
	}

public:

	Value *new_list_value();
	Node *new_node(const StaticString &name);
	Node *new_node(const StaticString &name, const ByReference &);
	Object *new_object();
	Value *new_object_value(Object *object);
	Value *new_value(const StaticString &text);
	Value *new_value(const StaticString &name, const ByReference &);

private:

	typedef Pool<Object> Objects;
	typedef Pool<Node>   Nodes;
	typedef Pool<Value>  Values;

private:

	Document  *_document;
	Allocator *_allocator;
	String     _buffer;
	Objects    _objects;
	Nodes      _nodes;
	Values     _values;
};

} // namespace Ion

} // namespace Yttrium

#endif // __ION_DOCUMENT_H
