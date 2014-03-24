#ifndef __ION_DOCUMENT_H
#define __ION_DOCUMENT_H

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/value.h>
#include <yttrium/pool.h>

namespace Yttrium
{

namespace Ion
{

class Y_PRIVATE Document::Private
{
	friend Document;

public:

	Private(Document* document, Allocator* allocator)
		: _document(document)
		, _allocator(allocator)
		, _buffer(allocator)
		, _objects(32, allocator)
		, _nodes(32, allocator)
		, _values(32, allocator)
	{
		// TODO: Get rid of magic numbers above.
	}

	Value* new_list_value();
	Node* new_node(const StaticString& name);
	Node* new_node(const StaticString& name, const ByReference&);
	Object* new_object();
	Value* new_object_value(Object* object);
	Value* new_value(const StaticString& text);
	Value* new_value(const StaticString& name, const ByReference&);

private:

	Document*    _document;
	Allocator*   _allocator;
	String       _buffer;
	Pool<Object> _objects;
	Pool<Node>   _nodes;
	Pool<Value>  _values;
};

} // namespace Ion

} // namespace Yttrium

#endif // __ION_DOCUMENT_H
