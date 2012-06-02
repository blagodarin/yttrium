#include <Yttrium/ion/document.hpp>

#include <Yttrium/ion/list.hpp>
#include <Yttrium/ion/node.hpp>
#include <Yttrium/ion/value.hpp>
#include <Yttrium/package.hpp>

#include "../base/file.hpp"

#include "parser.hpp"

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

Node *Document::new_node(const StaticString &name, const String::Reference &)
{
	return new(_nodes.allocate()) Node(this, name, String::Ref);
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

Value *Document::new_value(const StaticString &name, const String::Reference &)
{
	return new(_values.allocate()) Value(this, name, String::Ref);
}

} // namespace Ion

} // namespace Yttrium
