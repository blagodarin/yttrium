#include <Yttrium/ion/object.h>

#include <Yttrium/ion/document.h>

namespace Yttrium
{

namespace Ion
{

Node *Object::append(const StaticString &name)
{
	Node *node = _document.new_node(name);
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

void Object::concatenate(const Object &object)
{
	for (Object::ConstRange r = object.nodes(); !r.is_empty(); r.pop_first())
	{
		append(r.first());
	}
}

bool Object::contains(const StaticString &name)
{
	return _node_map.find(String(name, ByReference())) != _node_map.end();
}

const Node *Object::first() const
{
	return _nodes.empty() ? nullptr : _nodes.front();
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
	return nullptr;
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
	return (_nodes.empty() ? nullptr : _nodes.back());
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
	return nullptr;
}

void Object::to_string(String *result, int indentation) const
{
	to_string(result, indentation, false);
}

String Object::to_string(int indentation, Allocator *allocator) const
{
	String result(allocator ? allocator : _document._allocator);

	to_string(&result, indentation);
	return result;
}

Object::Object(Document *document)
	: _document(*document)
{
}

Node *Object::append(const StaticString &name, const ByReference &)
{
	Node *node = _document.new_node(name, ByReference());
	_nodes.push_back(node);
	_node_map[node->_name].push_back(node); // NOTE: Possible Allocator-less String construction.
	return node;
}

void Object::clear()
{
	_node_map.clear();
	_nodes.clear();
}

void Object::to_string(String *result, int indentation, bool document) const
{
	if (indentation < 0)
	{
		if (!document)
		{
			result->append('{');
		}

		for (Nodes::const_iterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			if (i != _nodes.begin())
			{
				result->append(' ');
			}
			(*i)->to_string(result, indentation);
		}

		if (!document)
		{
			result->append('}');
		}
	}
	else
	{
		if (!document)
		{
			result->append('\n').append('\t', indentation).append(S("{\n"));
		}

		int node_indentation = (document ? indentation : indentation + 1);

		for (Nodes::const_iterator i = _nodes.begin(); i != _nodes.end(); ++i)
		{
			(*i)->to_string(result, node_indentation);
			result->append('\n');
		}

		if (!document)
		{
			result->append('\t', indentation).append('}');
		}
	}
}

} // namespace Ion

} // namespace Yttrium
