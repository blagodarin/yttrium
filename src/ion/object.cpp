#include <yttrium/ion/object.h>

#include "document.h"

namespace Yttrium
{
	IonNode* IonObject::append(const StaticString& name)
	{
		IonNode* node = _document.new_node(name);
		_nodes.emplace_back(node);
		_node_map.emplace(node->_name, StdVector<IonNode*>(*_document.allocator())).first->second.emplace_back(node);
		return node;
	}

	IonObjectIterator IonObject::begin() const
	{
		return IonObjectIterator(_nodes.empty() ? nullptr : &_nodes.front());
	}

	bool IonObject::contains(const StaticString& name) const
	{
		return _node_map.find(String(name, ByReference())) != _node_map.end();
	}

	IonObjectIterator IonObject::end() const
	{
		return IonObjectIterator(_nodes.empty() ? nullptr : &_nodes.back() + 1);
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
		return IonDocumentPrivate::null_node;
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
		return IonDocumentPrivate::null_node;
	}

	IonObjectReverseIterator IonObject::rbegin() const
	{
		return IonObjectReverseIterator(_nodes.empty() ? nullptr : &_nodes.back());
	}

	IonObjectReverseIterator IonObject::rend() const
	{
		return IonObjectReverseIterator(_nodes.empty() ? nullptr : &_nodes.front() - 1);
	}

	IonObject::IonObject(IonDocumentPrivate& document)
		: _document(document)
		, _nodes(*_document.allocator())
		, _node_map(*_document.allocator())
	{
	}

	IonNode* IonObject::append(const StaticString& name, const ByReference&)
	{
		IonNode* node = _document.new_node(name, ByReference());
		_nodes.emplace_back(node);
		_node_map.emplace(node->_name, StdVector<IonNode*>(*_document.allocator())).first->second.emplace_back(node);
		return node;
	}

	void IonObject::clear()
	{
		_node_map.clear();
		_nodes.clear();
	}
}
