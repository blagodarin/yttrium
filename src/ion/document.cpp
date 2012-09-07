#include <Yttrium/ion/document.h>

#include <Yttrium/ion/list.h>
#include <Yttrium/ion/node.h>
#include <Yttrium/ion/value.h>
#include <Yttrium/package.h>

#include "../base/file.h"

#include "parser.h"

namespace Yttrium
{

namespace Ion
{

Document::Document(Allocator *allocator)
	: Object(this)
	, _allocator(allocator)
	, _buffer(allocator)
	, _objects(32, allocator)
	, _nodes(32, allocator)
	, _values(32, allocator)
{
}

Document::Document(const Document &document, Allocator *allocator)
	//: Document(allocator ? allocator : document._allocator) // TODO: Uncomment.
	: Object(this)
	, _allocator(allocator ? allocator : document._allocator)
	, _buffer(_allocator)
	, _objects(32, _allocator)
	, _nodes(32, _allocator)
	, _values(32, _allocator)
{
	concatenate(document);
}

void Document::clear()
{
	Object::clear();
	_buffer.clear();
}

bool Document::load(const StaticString &name)
{
	clear();

	return File(name, _allocator).read_all(&_buffer)
		&& Parser(this).parse(_buffer, name);
}

void Document::save(const StaticString &name, int indentation) const
{
	StaticFile file(name, File::Write, _allocator);

	if (!file.is_opened())
	{
		// TODO: Log the situation.
	}
	else
	{
		String buffer(_allocator);

		to_string(&buffer, (indentation > 0 ? 0 : indentation), true);

		file.truncate();
		file.write(buffer.text(), buffer.size());
	}
}

Value *Document::new_list_value()
{
	return new(_values.allocate()) Value(this);
}

Node *Document::new_node(const StaticString &name)
{
	return new(_nodes.allocate()) Node(this, name);
}

Node *Document::new_node(const StaticString &name, const ByReference &)
{
	return new(_nodes.allocate()) Node(this, name, ByReference());
}

Object *Document::new_object()
{
	return new(_objects.allocate()) Object(this);
}

Value *Document::new_object_value(Object *object)
{
	return new(_values.allocate()) Value(this, object);
}

Value *Document::new_value(const StaticString &name)
{
	return new(_values.allocate()) Value(this, name);
}

Value *Document::new_value(const StaticString &name, const ByReference &)
{
	return new(_values.allocate()) Value(this, name, ByReference());
}

} // namespace Ion

} // namespace Yttrium
