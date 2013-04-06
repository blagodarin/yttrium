#include <Yttrium/ion/object.h>

#include "document.h"
#include "node.h"

namespace Yttrium
{

namespace Ion
{

Node *Object::append(const StaticString &name)
{
	Node *node = _document->_private->new_node(name);
	_nodes.push_back(node);
	_node_map[node->_name].push_back(node);
	return node;
}

Node *Object::append(const Node &node)
{
	Node *new_node = append(node.name());
	new_node->concatenate(node);
	return new_node;
}

Object::ConstIterator Object::begin() const noexcept
{
	return ConstIterator(_nodes.empty() ? nullptr : &_nodes.front());
}

void Object::concatenate(const Object &object)
{
	for (const Node &node: object)
	{
		append(node);
	}
}

bool Object::contains(const StaticString &name)
{
	return _node_map.find(String(name, ByReference())) != _node_map.end();
}

Object::ConstIterator Object::end() const noexcept
{
	return ConstIterator(_nodes.empty() ? nullptr : &_nodes.back() + 1);
}

const Node *Object::first() const
{
	return _nodes.empty() ? &null_node : _nodes.front();
}

const Node *Object::first(const StaticString &name) const
{
	NodeMap::const_iterator i = _node_map.find(String(name, ByReference()));
	if (i != _node_map.end())
	{
		const Nodes &nodes = i->second;
		if (!nodes.empty())
		{
			return nodes.front();
		}
	}
	return &null_node;
}

Object::ConstRange Object::nodes() const
{
	if (_nodes.empty())
	{
		return ConstRange();
	}

	return ConstRange(&_nodes.front(), &_nodes.back());
}

Object::ConstRange Object::nodes(const StaticString &name) const
{
	NodeMap::const_iterator i = _node_map.find(String(name, ByReference()));
	if (i != _node_map.end())
	{
		const Nodes &nodes = i->second;
		if (!nodes.empty())
		{
			return ConstRange(&nodes.front(), &nodes.back());
		}
	}
	return ConstRange();
}

const Node *Object::last() const
{
	return _nodes.empty() ? &null_node : _nodes.back();
}

const Node *Object::last(const StaticString &name) const
{
	NodeMap::const_iterator i = _node_map.find(String(name, ByReference()));
	if (i != _node_map.end())
	{
		const Nodes &nodes = i->second;
		if (!nodes.empty())
		{
			return nodes.back();
		}
	}
	return &null_node;
}

bool Object::last(const StaticString &name, const Node **node) const
{
	const Node *last_node = last(name);
	if (last_node->exists())
	{
		*node = last_node;
		return true;
	}
	return false;
}

bool Object::last(const StaticString &name, const StaticString **string) const
{
	const Node *last_node = last(name);
	return last_node->exists() && !last_node->is_empty() && last_node->first()->get(string);
}

void Object::serialize(String *result, int indentation) const
{
	serialize(result, indentation, false);
}

String Object::serialize(int indentation, Allocator *allocator) const
{
	String result(allocator ? allocator : _document->allocator());

	serialize(&result, indentation);
	return result;
}

Object::Object(Document *document)
	: _document(document)
{
}

Node *Object::append(const StaticString &name, const ByReference &)
{
	Node *node = _document->_private->new_node(name, ByReference());
	_nodes.push_back(node);
	_node_map[node->_name].push_back(node); // NOTE: Possible Allocator-less String construction.
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

		for (Nodes::const_iterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			if (i != _nodes.begin())
			{
				result->append(' ');
			}
			(*i)->serialize(result, indentation);
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
