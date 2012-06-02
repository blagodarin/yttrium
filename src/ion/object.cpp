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

const Node *Object::first() const
{
	return (_nodes.empty() ? nullptr : _nodes.front());
}

const Node *Object::first(const StaticString &name) const
{
	NodeMap::const_iterator i = _node_map.find(String(name, String::Ref));
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
	return ConstRange(
		(_nodes.empty() ? nullptr : &_nodes.front()),
		_nodes.size());
}

Object::ConstRange Object::nodes(const StaticString &name) const
{
	NodeMap::const_iterator i = _node_map.find(name);
	if (i != _node_map.end())
	{
		const Nodes &nodes = i->second;
		if (!nodes.empty())
		{
			return ConstRange(&nodes.front(), nodes.size());
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
	NodeMap::const_iterator i = _node_map.find(String(name, String::Ref));
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

String Object::to_string(int indentation) const
{
	String result(_document._allocator);

	to_string(&result, indentation);
	return result;
}

Object::Object(Document *document)
	: _document(*document)
{
}

Node *Object::append(const StaticString &name, const String::Reference &)
{
	Node *node = _document.new_node(name, String::Ref);
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
			result->append('\n').append('\t', indentation).append(Y_S("{\n"));
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
