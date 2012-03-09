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
{
}

void Document::clear()
{
	Object::clear();

	_values.clear();
	_nodes.clear();
	_objects.clear();

	_buffer.clear();
}

bool Document::load(const StaticString &filename, FileSystem::Order order, FileSystem &fileSystem)
{
	clear();

	File file = fileSystem.open_file(filename, File::Read, order);
	if (!file)
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
	StaticFile file(filename, File::Write);

	if (!file)
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
	_values.push_back(Value(this));
	return &_values.back();
}

Node *Document::new_node(const StaticString &name)
{
	_nodes.push_back(Node(this, name));
	return &_nodes.back();
}

Node *Document::new_node(const StaticString &name, const String::Reference &)
{
	_nodes.push_back(Node(this, name, String::Ref));
	return &_nodes.back();
}

Object *Document::new_object()
{
	_objects.push_back(Object(this));
	return &_objects.back();
}

Value *Document::new_object_value(Object *object)
{
	_values.push_back(Value(this, object));
	return &_values.back();
}

Value *Document::new_value(const StaticString &name)
{
	_values.push_back(Value(this, name));
	return &_values.back();
}

Value *Document::new_value(const StaticString &name, const String::Reference &)
{
	_values.push_back(Value(this, name, String::Ref));
	return &_values.back();
}

} // namespace Ion

} // namespace Yttrium
