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

		inline void operator ++() noexcept;

		///

		inline const Node &operator *() const noexcept;

		///

		inline bool operator !=(ConstIterator iterator) const noexcept;

	private:

		const Node *const *_node;

		inline ConstIterator(const Node *const *node) noexcept;
	};

	///

	class ConstRange
	{
		friend Object;

	public:

		///

		inline ConstIterator begin() const noexcept;

		///

		inline ConstIterator end() const noexcept;

		///

		inline size_t size() const noexcept;

		///

		inline ConstReverseRange reverse() noexcept;

	private:

		const Node *const *_begin;
		const Node *const *_end;

		inline ConstRange(const Node *const *begin, const Node *const *end) noexcept;
	};

	///

	class ConstReverseIterator
	{
		friend ConstReverseRange;

	public:

		///

		inline void operator ++() noexcept;

		///

		inline const Node &operator *() const noexcept;

		///

		inline bool operator !=(ConstReverseIterator iterator) const noexcept;

	private:

		const Node *const *_node;

		inline ConstReverseIterator(const Node *const *node) noexcept;
	};

	///

	class ConstReverseRange
	{
		friend ConstRange;

	public:

		///

		inline ConstReverseIterator begin() const noexcept;

		///

		inline ConstReverseIterator end() const noexcept;

		///

		inline size_t size() const noexcept;

	private:

		const Node *const *_begin;
		const Node *const *_end;

		inline ConstReverseRange(const Node *const *begin, const Node *const *end) noexcept;
	};

public:

	///

	Node *append(const StaticString &name) noexcept;

	///

	Node *append(const Node &node) noexcept;

	///

	void concatenate(const Object &object) noexcept;

	///

	bool contains(const StaticString &name) noexcept;

	///

	inline const Document *document() const noexcept;

	///

	const Node &first() const noexcept;

	///

	const Node &first(const StaticString &name) const noexcept;

	///

	inline bool is_empty() const noexcept;

	///

	ConstRange nodes() const noexcept;

	///

	ConstRange nodes(const StaticString &name) const noexcept;

	///

	const Node &last() const noexcept;

	///

	const Node &last(const StaticString &name) const noexcept;

	///

	bool last(const StaticString &name, const Node **node) const noexcept;

	///

	bool last(const StaticString &name, const StaticString **string) const noexcept;

	///

	void serialize(String *result, int indentation = 0) const noexcept;

	///

	String serialize(int indentation = 0, Allocator *allocator = nullptr) const noexcept;

	///

	inline size_t size() const noexcept;

private:

	Y_PRIVATE Object(Document *document) noexcept;

private:

	Y_PRIVATE Node *append(const StaticString &name, const ByReference &) noexcept;
	Y_PRIVATE void clear() noexcept;
	Y_PRIVATE void serialize(String *result, int indentation, bool is_document) const noexcept;

private:

	Document* _document;
	std::vector<Node*> _nodes;
	std::map<String, std::vector<Node*>> _node_map;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Object::ConstIterator::operator ++() noexcept
{
	++_node;
}

const Node &Object::ConstIterator::operator *() const noexcept
{
	return **_node;
}

bool Object::ConstIterator::operator !=(ConstIterator iterator) const noexcept
{
	return _node != iterator._node;
}

Object::ConstIterator::ConstIterator(const Node *const *node) noexcept
	: _node(node)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Object::ConstIterator Object::ConstRange::begin() const noexcept
{
	return ConstIterator(_begin);
}

Object::ConstIterator Object::ConstRange::end() const noexcept
{
	return ConstIterator(_end);
}

size_t Object::ConstRange::size() const noexcept
{
	return _end - _begin;
}

Object::ConstReverseRange Object::ConstRange::reverse() noexcept
{
	return ConstReverseRange(_end - 1, _begin - 1);
}

Object::ConstRange::ConstRange(const Node *const *begin, const Node *const *end) noexcept
	: _begin(begin)
	, _end(end)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Object::ConstReverseIterator::operator ++() noexcept
{
	--_node;
}

const Node &Object::ConstReverseIterator::operator *() const noexcept
{
	return **_node;
}

bool Object::ConstReverseIterator::operator !=(ConstReverseIterator iterator) const noexcept
{
	return _node != iterator._node;
}

Object::ConstReverseIterator::ConstReverseIterator(const Node *const *node) noexcept
	: _node(node)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Object::ConstReverseIterator Object::ConstReverseRange::begin() const noexcept
{
	return _begin;
}

Object::ConstReverseIterator Object::ConstReverseRange::end() const noexcept
{
	return _end;
}

size_t Object::ConstReverseRange::size() const noexcept
{
	return _begin - _end;
}

Object::ConstReverseRange::ConstReverseRange(const Node *const *begin, const Node *const *end) noexcept
	: _begin(begin)
	, _end(end)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Document *Object::document() const noexcept
{
	return _document;
}

bool Object::is_empty() const noexcept
{
	return _nodes.empty();
}

size_t Object::size() const noexcept
{
	return _nodes.size();
}

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_OBJECT_H
