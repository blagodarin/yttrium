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

class Object: public Noncopyable
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
			, _size(0)
		{
		}

		///

		const Node &first() const noexcept
		{
			return **_first;
		}

		///

		bool is_empty() const noexcept
		{
			return !_size;
		}

		///

		void pop_first() noexcept
		{
			++_first;
			--_size;
		}

		///

		size_t size() const noexcept
		{
			return _size;
		}

	private:

		ConstRange(const Node *const *first, size_t size)
			: _first(first)
			, _size(size)
		{
		}

	private:

		const Node *const *_first;
		size_t             _size;
	};

public:

	///

	Node *append(const StaticString &name) noexcept;

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

	void to_string(String *result, int indentation = 0) const noexcept
	{
		to_string(result, indentation, false);
	}

	///

	String to_string(int indentation = 0) const noexcept; // NOTE: Wrong allocator?

private: // TODO: protected:

	Object(Document *document);

	Node *append(const StaticString &name, const String::Reference &);

	void clear();

	void to_string(String *result, int indentation, bool document) const noexcept;

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
