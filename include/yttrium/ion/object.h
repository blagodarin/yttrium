/// \file
/// \brief

#ifndef __Y_ION_OBJECT_H
#define __Y_ION_OBJECT_H

#include <yttrium/global.h>

#include <map>
#include <vector>

namespace Yttrium
{
	class Allocator;
	class ByReference;
	class IonDocument;
	class IonNode;
	class IonParser;
	class IonValue;
	class StaticString;
	class String;

	///
	class Y_API IonObject
	{
		friend IonDocument;
		friend IonParser;

	public:

		class ConstRange;
		class ConstReverseRange;

		///
		class ConstIterator
		{
			friend ConstRange;

		public:

			void operator++() { ++_node; }
			const IonNode& operator*() const { return **_node; }
			bool operator!=(ConstIterator iterator) const { return _node != iterator._node; }

		private:

			const IonNode* const* _node;

			ConstIterator(const IonNode* const* node): _node(node) {}
		};

		///
		class ConstRange
		{
			friend IonObject;

		public:

			ConstIterator begin() const { return ConstIterator(_begin); }
			ConstIterator end() const { return ConstIterator(_end); }
			size_t size() const { return _end - _begin; }
			ConstReverseRange reverse() const { return ConstReverseRange(_end - 1, _begin - 1); }

		private:

			const IonNode* const* _begin;
			const IonNode* const* _end;

			ConstRange(const IonNode* const* begin, const IonNode* const* end): _begin(begin), _end(end) {}
		};

		///
		class ConstReverseIterator
		{
			friend ConstReverseRange;

		public:

			void operator++() { --_node; }
			const IonNode& operator*() const { return **_node; }
			bool operator!=(ConstReverseIterator iterator) const { return _node != iterator._node; }

		private:

			const IonNode* const* _node;

			ConstReverseIterator(const IonNode* const* node): _node(node) {}
		};

		///
		class ConstReverseRange
		{
			friend ConstRange;

		public:

			ConstReverseIterator begin() const { return _begin; }
			ConstReverseIterator end() const { return _end; }
			size_t size() const { return _begin - _end; }

		private:

			const IonNode* const* _begin;
			const IonNode* const* _end;

			ConstReverseRange(const IonNode* const* begin, const IonNode* const* end): _begin(begin), _end(end) {}
		};

	public:

		///
		IonNode* append(const StaticString& name);

		///
		IonNode* append(const IonNode& node);

		///
		void concatenate(const IonObject& object);

		///
		bool contains(const StaticString& name);

		///
		const IonNode& first() const;

		///
		const IonNode& first(const StaticString& name) const;

		///
		ConstRange nodes() const;

		///
		ConstRange nodes(const StaticString& name) const;

		///
		const IonNode& last() const;

		///
		const IonNode& last(const StaticString& name) const;

		///
		bool last(const StaticString& name, const IonNode** node) const;

		///
		bool last(const StaticString& name, const StaticString** string) const;

		///
		void serialize(String* result, int indentation = 0) const;

		///
		String serialize(int indentation = 0, Allocator* allocator = nullptr) const;

		///
		size_t size() const { return _nodes.size(); }

		IonObject(const IonObject&) = delete;
		IonObject& operator=(const IonObject&) = delete;

	private:

		Y_PRIVATE IonObject(IonDocument* document);

		Y_PRIVATE IonNode* append(const StaticString& name, const ByReference&);
		Y_PRIVATE void clear();
		Y_PRIVATE void serialize(String* result, int indentation, bool is_document) const;

	private:

		IonDocument* _document;
		std::vector<IonNode*> _nodes;
		std::map<String, std::vector<IonNode*>> _node_map;
	};
}

#endif
