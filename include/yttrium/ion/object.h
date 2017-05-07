#ifndef _include_yttrium_ion_object_h_
#define _include_yttrium_ion_object_h_

#include <yttrium/api.h>
#include <yttrium/std/string_view.h>

#include <map>
#include <vector>

namespace Yttrium
{
	class ByReference;
	class IonDocumentImpl;
	class IonNode;
	class IonObjectIterator;
	class IonObjectReverseIterator;
	class IonParser;
	class IonValue;
	class String;

	///
	class Y_API IonObject
	{
		friend IonDocumentImpl;
		friend IonParser;

	public:

		///
		IonNode* append(std::string_view);

		///
		IonObjectIterator begin() const;

		///
		bool contains(std::string_view) const;

		///
		IonObjectIterator end() const;

		///
		const IonNode& last(std::string_view) const;

		///
		IonObjectReverseIterator rbegin() const;

		///
		IonObjectReverseIterator rend() const;

		///
		size_t size() const { return _nodes.size(); }

		IonObject(const IonObject&) = delete;
		IonObject& operator=(const IonObject&) = delete;

	private:
		explicit IonObject(IonDocumentImpl&);

		Y_PRIVATE IonNode* append(std::string_view, const ByReference&);

	private:
		IonDocumentImpl& _document;
		std::vector<IonNode*> _nodes;
		std::map<String, std::vector<IonNode*>> _node_map;
	};

	class IonObjectIterator
	{
		friend IonObject;
	public:
		void operator++() { ++_node; }
		const IonNode& operator*() const { return **_node; }
		const IonNode* operator->() const { return *_node; }
		bool operator==(IonObjectIterator iterator) const { return _node == iterator._node; }
		bool operator!=(IonObjectIterator iterator) const { return !operator==(iterator); }
	private:
		const IonNode* const* _node;
		explicit IonObjectIterator(const IonNode* const* node) : _node{node} {}
	};

	class IonObjectReverseIterator
	{
		friend IonObject;
	public:
		void operator++() { --_node; }
		const IonNode& operator*() const { return **_node; }
		const IonNode* operator->() const { return *_node; }
		bool operator==(IonObjectReverseIterator iterator) const { return _node == iterator._node; }
		bool operator!=(IonObjectReverseIterator iterator) const { return !operator==(iterator); }
	private:
		const IonNode* const* _node;
		explicit IonObjectReverseIterator(const IonNode* const* node) : _node{node} {}
	};
}

#endif
