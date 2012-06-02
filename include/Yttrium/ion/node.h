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

	StaticString name() const noexcept
	{
		return _name;
	}

	///

	void to_string(String *result, int indentation = 0) const noexcept;

	///

	String to_string(int indentation = 0) const noexcept
	{
		String result(_name.allocator()); // NOTE: Wrong allocator?

		to_string(&result, indentation);
		return result;
	}

private:

	Node(Document *document, const StaticString &name);

	Node(Document *document, const StaticString &name, const String::Reference &);

private:

	String  _name;
	Node   *_next;
};

} // namespace Ion

} // namespace Yttrium

#endif // __Y_ION_NODE_H
