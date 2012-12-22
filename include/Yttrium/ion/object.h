/// \file
/// \brief

#ifndef __Y_ION_OBJECT_H
#define __Y_ION_OBJECT_H

#include <Yttrium/ion/node.h> // NOTE: For inlines. =(
#include <Yttrium/ion/value.h> // NOTE: For inlines. =(
#include <Yttrium/noncopyable.h>
#include <Yttrium/static_string.h>

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

class Y_API Object: public Noncopyable
{
	friend class Document;
	friend class Parser;

public:

	///

	class ConstRange
	{
		friend class Object;

	public:

		///

		inline ConstRange() noexcept;

	public:

		///

		inline const Node &first() const noexcept;

		///

		inline bool is_empty() const noexcept;

		///

		inline const Node &last() const noexcept;

		///

		inline void pop_first() noexcept;

		///

		inline void pop_last() noexcept;

		///

		inline size_t size() const noexcept;

	private:

		inline ConstRange(const Node *const *first, const Node *const *last) noexcept;

	private:

		const Node *const *_first;
		const Node *const *_last;
	};

public:

	Object &operator =(const Object &) = delete;

	///

	Node *append(const StaticString &name) noexcept;

	///

	Node *append(const Node &node) noexcept;

	///

	void concatenate(const Object &object) noexcept;

	///

	bool contains(const StaticString &name) noexcept;

	///

	const Node *first() const noexcept;

	///

	const Node *first(const StaticString &name) const noexcept;

	///

	inline bool is_empty() const noexcept;

	///

	ConstRange nodes() const noexcept;

	///

	ConstRange nodes(const StaticString &name) const noexcept;

	///

	const Node *last() const noexcept;

	///

	const Node *last(const StaticString &name) const noexcept;

	///

	inline bool last(const StaticString &name, const Node **node) const noexcept;

	///

	inline bool last(const StaticString &name, const StaticString **string) const noexcept;

	///

	inline size_t size() const noexcept;

	///

	void to_string(String *result, int indentation = 0) const noexcept;

	///

	String to_string(int indentation = 0, Allocator *allocator = nullptr) const noexcept;

private:

	Y_PRIVATE Object(Document *document) noexcept;

private:

	Y_PRIVATE Node *append(const StaticString &name, const ByReference &) noexcept;
	Y_PRIVATE void clear() noexcept;
	Y_PRIVATE void to_string(String *result, int indentation, bool document) const noexcept;

private:

	typedef std::vector<Node *>     Nodes;
	typedef std::map<String, Nodes> NodeMap;

private:

	Document &_document;
	Nodes     _nodes;
	NodeMap   _node_map;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Object::ConstRange::ConstRange() noexcept
	: _first(nullptr)
	, _last(_first - 1)
{
}

const Node &Object::ConstRange::first() const noexcept
{
	return **_first;
}

bool Object::ConstRange::is_empty() const noexcept
{
	return _first > _last;
}

const Node &Object::ConstRange::last() const noexcept
{
	return **_last;
}

void Object::ConstRange::pop_first() noexcept
{
	++_first;
}

void Object::ConstRange::pop_last() noexcept
{
	--_last;
}

size_t Object::ConstRange::size() const noexcept
{
	return _last - _first + 1;
}

Object::ConstRange::ConstRange(const Node *const *first, const Node *const *last) noexcept
	: _first(first)
	, _last(last)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Object::is_empty() const noexcept
{
	return _nodes.empty();
}

bool Object::last(const StaticString &name, const Node **node) const noexcept
{
	const Node *last_node = last(name);
	if (last_node->exists())
	{
		*node = last_node;
		return true;
	}
	return false;
}

bool Object::last(const StaticString &name, const StaticString **string) const noexcept
{
	const Node *last_node = last(name);
	return last_node->exists() && !last_node->is_empty() && last_node->first()->get(string);
}

size_t Object::size() const noexcept
{
	return _nodes.size();
}

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_OBJECT_H
