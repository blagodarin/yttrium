/// \file
/// \brief

#ifndef __Y_ION_NODE_H
#define __Y_ION_NODE_H

#include <Yttrium/ion/list.h>
#include <Yttrium/string.h>

namespace Yttrium
{

namespace Ion
{

class Document;
class Object;

///

class Y_API Node: public List
{
	friend class Document;
	friend class Object;

public:

	///

	inline StaticString name() const noexcept;

	///

	void to_string(String *result, int indentation = 0) const noexcept;

	///

	String to_string(int indentation = 0, Allocator *allocator = nullptr) const noexcept;

private:

	Y_PRIVATE Node(Document *document, const StaticString &name) noexcept;
	Y_PRIVATE Node(Document *document, const StaticString &name, const ByReference &) noexcept;

private:

	String  _name;
	Node   *_next;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StaticString Node::name() const noexcept
{
	return _name;
}

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_NODE_H
