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

	inline String to_string(int indentation = 0) const noexcept;

private:

	Y_PRIVATE Node(Document *document, const StaticString &name);

	Y_PRIVATE Node(Document *document, const StaticString &name, const String::Reference &);

private:

	String  _name;
	Node   *_next;
};

////////////////////////////////////////////////////////////////////////////////

StaticString Node::name() const noexcept
{
	return _name;
}

String Node::to_string(int indentation) const noexcept
{
	String result(_name.allocator()); // NOTE: Wrong allocator?

	to_string(&result, indentation);
	return result;
}

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_NODE_H
