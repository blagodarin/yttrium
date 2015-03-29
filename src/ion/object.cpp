#include <yttrium/ion/object.h>

#include "document.h"
#include "node.h"

namespace Yttrium
{
	IonNode* IonObject::append(const StaticString& name)
	{
		IonNode* node = _document->_private->new_node(name);
		_nodes.push_back(node);
		_node_map[node->_name].push_back(node);
		return node;
	}

	IonNode* IonObject::append(const IonNode& node)
	{
		IonNode* new_node = append(node.name());
		new_node->concatenate(node);
		return new_node;
	}

	void IonObject::concatenate(const IonObject& object)
	{
		for (const IonNode& node: object.nodes())
			append(node);
	}

	bool IonObject::contains(const StaticString& name)
	{
		return _node_map.find(String(name, ByReference())) != _node_map.end();
	}

	const IonNode& IonObject::first() const
	{
		return _nodes.empty() ? Ion::null_node : *_nodes.front();
	}

	const IonNode& IonObject::first(const StaticString& name) const
	{
		const auto i = _node_map.find(String(name, ByReference()));
		if (i != _node_map.end())
		{
			const auto& nodes = i->second;
			if (!nodes.empty())
				return *nodes.front();
		}
		return Ion::null_node;
	}

	IonObject::ConstRange IonObject::nodes() const
	{
		return _nodes.empty() ? ConstRange(nullptr, nullptr) : ConstRange(&_nodes.front(), &_nodes.back() + 1);
	}

	IonObject::ConstRange IonObject::nodes(const StaticString& name) const
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

	const IonNode& IonObject::last() const
	{
		return _nodes.empty() ? Ion::null_node : *_nodes.back();
	}

	const IonNode& IonObject::last(const StaticString& name) const
	{
		const auto i = _node_map.find(String(name, ByReference()));
		if (i != _node_map.end())
		{
			const auto& nodes = i->second;
			if (!nodes.empty())
				return *nodes.back();
		}
		return Ion::null_node;
	}

	bool IonObject::last(const StaticString& name, const IonNode** node) const
	{
		const IonNode& last_node = last(name);
		if (last_node.exists())
		{
			*node = &last_node;
			return true;
		}
		return false;
	}

	bool IonObject::last(const StaticString& name, const StaticString** string) const
	{
		const IonNode& last_node = last(name);
		return last_node.exists() && !last_node.is_empty() && last_node.first()->get(string);
	}

	void IonObject::serialize(String* result, int indentation) const
	{
		serialize(result, indentation, false);
	}

	String IonObject::serialize(int indentation, Allocator* allocator) const
	{
		String result(allocator ? allocator : _document->allocator());
		serialize(&result, indentation);
		return result;
	}

	IonObject::IonObject(IonDocument* document)
		: _document(document)
	{
	}

	IonNode* IonObject::append(const StaticString& name, const ByReference&)
	{
		IonNode* node = _document->_private->new_node(name, ByReference());
		_nodes.push_back(node);
		_node_map[node->_name].push_back(node);
		return node;
	}

	void IonObject::clear()
	{
		_node_map.clear();
		_nodes.clear();
	}

	void IonObject::serialize(String *result, int indentation, bool is_document) const
	{
		if (indentation < 0)
		{
			if (!is_document)
				result->append('{');

			bool need_separator = false;

			for (const IonNode* node : _nodes)
			{
				if (need_separator)
					result->append(' ');
				node->serialize(result, indentation);
				need_separator = node->is_empty();
			}

			if (!is_document)
				result->append('}');
		}
		else
		{
			if (!is_document)
				result->append('\n').append('\t', indentation).append(S("{\n"));

			int node_indentation = indentation + !is_document;

			for (const IonNode* node : _nodes)
			{
				node->serialize(result, node_indentation);
				result->append('\n');
			}

			if (!is_document)
				result->append('\t', indentation).append('}');
		}
	}
}
