/// \file
/// \brief

#ifndef _include_yttrium_ion_object_h_
#define _include_yttrium_ion_object_h_

#include <yttrium/global.h>

#include <map>
#include <vector>

namespace Yttrium
{
	class ByReference;
	class IonDocumentPrivate;
	class IonNode;
	class IonObjectIterator;
	class IonObjectReverseIterator;
	class IonParser;
	class IonValue;
	class StaticString;
	class String;

	///
	class Y_API IonObject
	{
		friend IonDocumentPrivate;
		friend IonParser;

	public:

		///
		IonNode* append(const StaticString& name);

		///
		IonObjectIterator begin() const;

		///
		bool contains(const StaticString& name) const;

		///
		IonObjectIterator end() const;

		///
		const IonNode& first(const StaticString& name) const;

		///
		const IonNode& last(const StaticString& name) const;

		///
		IonObjectReverseIterator rbegin() const;

		///
		IonObjectReverseIterator rend() const;

		///
		size_t size() const { return _nodes.size(); }

		IonObject(const IonObject&) = delete;
		IonObject& operator=(const IonObject&) = delete;

	private:

		IonObject(IonDocumentPrivate& document) : _document(document) {}

		Y_PRIVATE IonNode* append(const StaticString& name, const ByReference&);
		Y_PRIVATE void clear();

	private:

		IonDocumentPrivate& _document;
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
		IonObjectIterator(const IonNode* const* node): _node(node) {}
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
		IonObjectReverseIterator(const IonNode* const* node): _node(node) {}
	};
}

#endif
