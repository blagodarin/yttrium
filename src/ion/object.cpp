#include <yttrium/ion/object.h>

#include "document.h"

namespace Yttrium
{
	IonNode* IonObject::append(std::string_view name)
	{
		const auto node = _document.new_node(name);
		_nodes.emplace_back(node);
		_node_map[node->_name].emplace_back(node);
		return node;
	}

	IonObjectIterator IonObject::begin() const
	{
		return IonObjectIterator{_nodes.empty() ? nullptr : &_nodes.front()};
	}

	bool IonObject::contains(std::string_view name) const
	{
		return _node_map.find({name, ByReference{}}) != _node_map.end();
	}

	IonObjectIterator IonObject::end() const
	{
		return IonObjectIterator(_nodes.empty() ? nullptr : &_nodes.back() + 1);
	}

	const IonNode& IonObject::last(std::string_view name) const
	{
		const auto i = _node_map.find({name, ByReference{}});
		if (i != _node_map.end())
		{
			const auto& nodes = i->second;
			if (!nodes.empty())
				return *nodes.back();
		}
		return IonDocumentImpl::null_node;
	}

	IonObjectReverseIterator IonObject::rbegin() const
	{
		return IonObjectReverseIterator{_nodes.empty() ? nullptr : &_nodes.back()};
	}

	IonObjectReverseIterator IonObject::rend() const
	{
		return IonObjectReverseIterator{_nodes.empty() ? nullptr : &_nodes.front() - 1};
	}

	IonObject::IonObject(IonDocumentImpl& document)
		: _document{document}
	{
	}

	IonNode* IonObject::append(std::string_view name, const ByReference&)
	{
		const auto node = _document.new_node(name, ByReference{});
		_nodes.emplace_back(node);
		_node_map[node->_name].emplace_back(node);
		return node;
	}
}
