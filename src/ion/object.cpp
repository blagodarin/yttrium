#include <yttrium/ion/object.h>

#include "document.h"
#include "node.h"

namespace Yttrium
{

namespace Ion
{

Node* Object::append(const StaticString& name)
{
	Node* node = _document->_private->new_node(name);
	_nodes.push_back(node);
	_node_map[node->_name].push_back(node);
	return node;
}

Node* Object::append(const Node& node)
{
	Node *new_node = append(node.name());
	new_node->concatenate(node);
	return new_node;
}

void Object::concatenate(const Object& object)
{
	for (const Node& node: object.nodes())
		append(node);
}

bool Object::contains(const StaticString& name)
{
	return _node_map.find(String(name, ByReference())) != _node_map.end();
}

const Node& Object::first() const
{
	return _nodes.empty() ? null_node : *_nodes.front();
}

const Node& Object::first(const StaticString& name) const
{
	const auto i = _node_map.find(String(name, ByReference()));
	if (i != _node_map.end())
	{
		const auto& nodes = i->second;
		if (!nodes.empty())
			return *nodes.front();
	}
	return null_node;
}

Object::ConstRange Object::nodes() const
{
	if (_nodes.empty())
		return ConstRange(nullptr, nullptr);

	return ConstRange(&_nodes.front(), &_nodes.back() + 1);
}

Object::ConstRange Object::nodes(const StaticString& name) const
{
	const auto i = _node_map.find(String(name, ByReference()));
	if (i != _node_map.end())
	{
		const auto& nodes = i->second;
		if (!nodes.empty())
			return ConstRange(&nodes.front(), &nodes.back() + 1);
	}
	return ConstRange(nullptr, nullptr);
}

const Node& Object::last() const
{
	return _nodes.empty() ? null_node : *_nodes.back();
}

const Node& Object::last(const StaticString& name) const
{
	const auto i = _node_map.find(String(name, ByReference()));
	if (i != _node_map.end())
	{
		const auto& nodes = i->second;
		if (!nodes.empty())
			return *nodes.back();
	}
	return null_node;
}

bool Object::last(const StaticString& name, const Node** node) const
{
	const Node& last_node = last(name);
	if (last_node.exists())
	{
		*node = &last_node;
		return true;
	}
	return false;
}

bool Object::last(const StaticString& name, const StaticString** string) const
{
	const Node &last_node = last(name);
	return last_node.exists() && !last_node.is_empty() && last_node.first()->get(string);
}

void Object::serialize(String* result, int indentation) const
{
	serialize(result, indentation, false);
}

String Object::serialize(int indentation, Allocator* allocator) const
{
	String result(allocator ? allocator : _document->allocator());

	serialize(&result, indentation);
	return result;
}

Object::Object(Document* document)
	: _document(document)
{
}

Node* Object::append(const StaticString &name, const ByReference &)
{
	Node* node = _document->_private->new_node(name, ByReference());
	_nodes.push_back(node);
	_node_map[node->_name].push_back(node);
	return node;
}

void Object::clear()
{
	_node_map.clear();
	_nodes.clear();
}

void Object::serialize(String *result, int indentation, bool is_document) const
{
	if (indentation < 0)
	{
		if (!is_document)
		{
			result->append('{');
		}

		bool need_separator = false;

		for (auto i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			if (need_separator)
			{
				result->append(' ');
			}
			(*i)->serialize(result, indentation);
			need_separator = (*i)->is_empty();
		}

		if (!is_document)
		{
			result->append('}');
		}
	}
	else
	{
		if (!is_document)
		{
			result->append('\n').append('\t', indentation).append(S("{\n"));
		}

		int node_indentation = indentation + !is_document;

		for (const Node *node: _nodes)
		{
			node->serialize(result, node_indentation);
			result->append('\n');
		}

		if (!is_document)
		{
			result->append('\t', indentation).append('}');
		}
	}
}

} // namespace Ion

} // namespace Yttrium
