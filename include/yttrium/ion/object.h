/// \file
/// \brief

#ifndef __Y_ION_OBJECT_H
#define __Y_ION_OBJECT_H

#include <yttrium/static_string.h>

#include <map>
#include <vector>

namespace Yttrium
{

class String;

namespace Ion
{

class Document;
class Node;
class Parser;
class Value;

///

class Y_API Object
{
	friend Document;
	friend Parser;

	Y_NONCOPYABLE(Object);

public:

	class ConstRange;
	class ConstReverseRange;

	///

	class ConstIterator
	{
		friend ConstRange;

	public:

		///

		inline void operator ++();

		///

		inline const Node &operator *() const;

		///

		inline bool operator !=(ConstIterator iterator) const;

	private:

		const Node *const *_node;

		inline ConstIterator(const Node *const *node);
	};

	///

	class ConstRange
	{
		friend Object;

	public:

		///

		inline ConstIterator begin() const;

		///

		inline ConstIterator end() const;

		///

		inline size_t size() const;

		///

		inline ConstReverseRange reverse();

	private:

		const Node *const *_begin;
		const Node *const *_end;

		inline ConstRange(const Node *const *begin, const Node *const *end);
	};

	///

	class ConstReverseIterator
	{
		friend ConstReverseRange;

	public:

		///

		inline void operator ++();

		///

		inline const Node &operator *() const;

		///

		inline bool operator !=(ConstReverseIterator iterator) const;

	private:

		const Node *const *_node;

		inline ConstReverseIterator(const Node *const *node);
	};

	///

	class ConstReverseRange
	{
		friend ConstRange;

	public:

		///

		inline ConstReverseIterator begin() const;

		///

		inline ConstReverseIterator end() const;

		///

		inline size_t size() const;

	private:

		const Node *const *_begin;
		const Node *const *_end;

		inline ConstReverseRange(const Node *const *begin, const Node *const *end);
	};

public:

	///

	Node *append(const StaticString &name);

	///

	Node *append(const Node &node);

	///

	void concatenate(const Object &object);

	///

	bool contains(const StaticString &name);

	///

	inline const Document *document() const;

	///

	const Node &first() const;

	///

	const Node &first(const StaticString &name) const;

	///

	inline bool is_empty() const;

	///

	ConstRange nodes() const;

	///

	ConstRange nodes(const StaticString &name) const;

	///

	const Node &last() const;

	///

	const Node &last(const StaticString &name) const;

	///

	bool last(const StaticString &name, const Node **node) const;

	///

	bool last(const StaticString &name, const StaticString **string) const;

	///

	void serialize(String *result, int indentation = 0) const;

	///

	String serialize(int indentation = 0, Allocator *allocator = nullptr) const;

	///

	inline size_t size() const;

private:

	Y_PRIVATE Object(Document *document);

private:

	Y_PRIVATE Node *append(const StaticString &name, const ByReference &);
	Y_PRIVATE void clear();
	Y_PRIVATE void serialize(String *result, int indentation, bool is_document) const;

private:

	Document* _document;
	std::vector<Node*> _nodes;
	std::map<String, std::vector<Node*>> _node_map;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Object::ConstIterator::operator ++()
{
	++_node;
}

const Node &Object::ConstIterator::operator *() const
{
	return **_node;
}

bool Object::ConstIterator::operator !=(ConstIterator iterator) const
{
	return _node != iterator._node;
}

Object::ConstIterator::ConstIterator(const Node *const *node)
	: _node(node)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Object::ConstIterator Object::ConstRange::begin() const
{
	return ConstIterator(_begin);
}

Object::ConstIterator Object::ConstRange::end() const
{
	return ConstIterator(_end);
}

size_t Object::ConstRange::size() const
{
	return _end - _begin;
}

Object::ConstReverseRange Object::ConstRange::reverse()
{
	return ConstReverseRange(_end - 1, _begin - 1);
}

Object::ConstRange::ConstRange(const Node *const *begin, const Node *const *end)
	: _begin(begin)
	, _end(end)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Object::ConstReverseIterator::operator ++()
{
	--_node;
}

const Node &Object::ConstReverseIterator::operator *() const
{
	return **_node;
}

bool Object::ConstReverseIterator::operator !=(ConstReverseIterator iterator) const
{
	return _node != iterator._node;
}

Object::ConstReverseIterator::ConstReverseIterator(const Node *const *node)
	: _node(node)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Object::ConstReverseIterator Object::ConstReverseRange::begin() const
{
	return _begin;
}

Object::ConstReverseIterator Object::ConstReverseRange::end() const
{
	return _end;
}

size_t Object::ConstReverseRange::size() const
{
	return _begin - _end;
}

Object::ConstReverseRange::ConstReverseRange(const Node *const *begin, const Node *const *end)
	: _begin(begin)
	, _end(end)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Document *Object::document() const
{
	return _document;
}

bool Object::is_empty() const
{
	return _nodes.empty();
}

size_t Object::size() const
{
	return _nodes.size();
}

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_OBJECT_H
