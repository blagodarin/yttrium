#include <Yttrium/ion/document.hpp>

#include <Yttrium/ion/list.hpp>
#include <Yttrium/ion/node.hpp>
#include <Yttrium/ion/value.hpp>

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

bool Document::load(const StaticString &filename, FileSystem::Order order, FileSystem &fileSystem)
{
	clear();

	File file = fileSystem.open_file(filename, File::Read, order);
	if (!file.is_opened())
	{
		return false;
	}

	_buffer.resize(file.size()); // NOTE: May result in unexpected behavior.
	if (!file.read(_buffer.text(), _buffer.size()))
	{
		return false;
	}
	file.close();

	return Parser(this).parse(_buffer, filename);
}

void Document::save(const StaticString &filename, int indentation) const
{
	StaticFile file(filename, File::Write, _allocator);

	if (!file.is_opened())
	{
		// TODO: Log the situation.
	}
	else
	{
		String buffer(_allocator);

		to_string(&buffer, (indentation > 0 ? 0 : indentation), true);
		//buffer.append('\n');

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
