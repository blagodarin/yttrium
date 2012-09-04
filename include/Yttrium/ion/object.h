/// \file
/// \brief

#ifndef __Y_ION_OBJECT_H
#define __Y_ION_OBJECT_H

#include <Yttrium/noncopyable.h>
#include <Yttrium/string.h>

#include <map>
#include <vector>

namespace Yttrium
{

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

		ConstRange() noexcept
			: _first(nullptr)
			, _last(_first - 1)
		{
		}

	public:

		///

		const Node &first() const noexcept
		{
			return **_first;
		}

		///

		bool is_empty() const noexcept
		{
			return _first > _last;
		}

		///

		const Node &last() const noexcept
		{
			return **_last;
		}

		///

		void pop_first() noexcept
		{
			++_first;
		}

		///

		void pop_last() noexcept
		{
			--_last;
		}

		///

		size_t size() const noexcept
		{
			return _last - _first + 1;
		}

	private:

		ConstRange(const Node *const *first, const Node *const *last)
			: _first(first)
			, _last(last)
		{
		}

	private:

		const Node *const *_first;
		const Node *const *_last;
	};

public:

	///

	Node *append(const StaticString &name) noexcept;

	///

	Node *append(const Node *node) noexcept;

	///

	bool contains(const StaticString &name) noexcept;

	///

	const Node *first() const noexcept;

	///

	const Node *first(const StaticString &name) const noexcept;

	///

	ConstRange nodes() const noexcept;

	///

	ConstRange nodes(const StaticString &name) const noexcept;

	///

	const Node *last() const noexcept;

	///

	const Node *last(const StaticString &name) const noexcept;

	///

	void to_string(String *result, int indentation = 0) const noexcept;

	///

	String to_string(int indentation = 0, Allocator *allocator = nullptr) const noexcept;

private: // TODO: protected:

	Y_PRIVATE Object(Document *document);

	Y_PRIVATE Node *append(const StaticString &name, const String::Reference &);

	Y_PRIVATE void clear();

	Y_PRIVATE void to_string(String *result, int indentation, bool document) const;

private:

	typedef std::vector<Node *>     Nodes;
	typedef std::map<String, Nodes> NodeMap;

private:

	Document &_document;
	Nodes     _nodes;
	NodeMap   _node_map;
};

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_OBJECT_H
